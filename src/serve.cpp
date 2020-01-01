
#include <new>

#include <gsl/gsl>

#include <grpcpp/grpcpp.h>
#include <service.grpc.pb.h>

#ifdef _LIBCPP_HAS_NO_COROUTINES
#undef _LIBCPP_HAS_NO_COROUTINES
#endif

#if __has_include(<coroutine>)
#include <coroutine>
using namespace std;

#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
using namespace std;
using namespace std::experimental;

#endif

struct promise_nn {
    auto initial_suspend() noexcept {
        return suspend_never{};
    }
    auto final_suspend() noexcept {
        return suspend_never{};
    }
    void unhandled_exception() noexcept(false) {
        throw;
    }
};

using rpc_request_t = function<void(coroutine_handle<void>)>;

class rpc_context_t {
  protected:
    rpc_request_t request{};

  public:
    void set_failed() noexcept {
        request = nullptr;
    }

  public:
    constexpr bool await_ready() noexcept {
        return false;
    }
    void await_suspend(coroutine_handle<void> coro) noexcept(false) {
        return request(coro);
    }
    bool await_resume() noexcept {
        return static_cast<bool>(request);
    }
};

class rpc_routine_t final {
  public:
    class promise_type final : public promise_nn, public rpc_context_t {
      public:
        void return_value(grpc::Status) noexcept {
            // do nothing with the value.
        }
        auto await_transform(rpc_request_t&& fn) noexcept -> rpc_context_t& {
            this->request = move(fn);
            return *this;
        }

        auto get_return_object() noexcept {
            return rpc_routine_t{this};
        }
    };

  public:
    explicit rpc_routine_t(promise_type*) noexcept(false){
        // if (p)
        //     handle = coroutine_handle<promise_type>::from_promise(*p);
    };
};

void serve_user_data(void* user_data, bool ok) noexcept(false) {
    using promise_t = rpc_routine_t::promise_type;
    auto task = coroutine_handle<promise_t>::from_address(user_data);
    if (ok == false)
        task.promise().set_failed();

    if (task.done() == false)
        task.resume();
}

bool is_shutdowned() noexcept;

template <typename Fn>
auto until(Fn&& fn) -> rpc_request_t {
    return fn;
}

using namespace grpc;

auto serve(v1::Worker::AsyncService& service, //
           v1::Request& req, v1::Response& res) -> StatusCode {
    res.set_blob(req.blob());
    return StatusCode::OK;
}

auto serve_method1(grpc_impl::ServerCompletionQueue& queue,
                   v1::Worker::AsyncService& service) noexcept(false)
    -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method1(queue, service);
    });

    // prepare to receive 1 request
    ServerContext ctx{};
    ServerAsyncResponseWriter<v1::Response> responder{&ctx};
    v1::Request req{};

    if (co_await until([&](coroutine_handle<void> coro) {
            service.RequestMethod1(&ctx, &req, &responder, &queue, &queue,
                                   coro.address());
        }) == false)
        co_return Status{StatusCode::CANCELLED, "service.RequestMethod1"};

    v1::Response res{};
    grpc::Status status{serve(service, req, res),
                        v1::Worker::service_full_name()};

    co_await until([&](coroutine_handle<void> coro) {
        responder.Finish(res, status, coro.address());
    });
    co_return status;
}

auto serve_method2(grpc_impl::ServerCompletionQueue& queue,
                   v1::Worker::AsyncService& service) noexcept(false)
    -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method2(queue, service);
    });

    ServerContext ctx{};
    ServerAsyncWriter<v1::Response> writer{&ctx};
    v1::Request req{};

    co_await until([&](coroutine_handle<void> coro) {
        service.RequestMethod2(&ctx, &req, &writer, &queue, &queue,
                               coro.address());
    });

    v1::Response res{};
    grpc::Status status{};

    for (auto i = 0u; i < 1u; ++i) {
        co_await until([&](coroutine_handle<void> coro) {
            writer.Write(res, coro.address());
        });
        status = grpc::Status{serve(service, req, res), //
                              v1::Worker::service_full_name()};
    }
    co_await until([&](coroutine_handle<void> coro) {
        writer.Finish(status, coro.address());
    });
    co_return status;
}

auto serve_method3(grpc_impl::ServerCompletionQueue& queue,
                   v1::Worker::AsyncService& service) noexcept(false)
    -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method3(queue, service);
    });

    ServerContext ctx{};
    ServerAsyncReader<v1::Response, v1::Request> reader{&ctx};

    co_await until([&](coroutine_handle<void> coro) {
        service.RequestMethod3(&ctx, &reader, &queue, &queue, coro.address());
    });

    v1::Request req{};
    v1::Response res{};
    grpc::Status status{};

    while (co_await until([&](coroutine_handle<void> coro) {
        reader.Read(&req, coro.address());
    })) {
        // will return the last one
        status = grpc::Status{serve(service, req, res), //
                              v1::Worker::service_full_name()};
    }
    co_await until([&](coroutine_handle<void> coro) {
        reader.Finish(res, status, coro.address());
    });
    co_return status;
}

auto serve_method4(grpc_impl::ServerCompletionQueue& queue,
                   v1::Worker::AsyncService& service) noexcept(false)
    -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method3(queue, service);
    });

    ServerContext ctx{};
    ServerAsyncReaderWriter<v1::Response, v1::Request> link{&ctx};

    co_await until([&](coroutine_handle<void> coro) {
        service.RequestMethod4(&ctx, &link, &queue, &queue, coro.address());
    });

    v1::Request req{};
    v1::Response res{};
    grpc::Status status{};

    for (; // read one time ...
         co_await until([&](coroutine_handle<void> coro) {
             link.Read(&req, coro.address());
         });
         // write one time ...
         co_await until([&](coroutine_handle<void> coro) {
             link.Write(res, coro.address());
         })) {
        status = grpc::Status{serve(service, req, res), //
                              v1::Worker::service_full_name()};
    }
    co_await until([&](coroutine_handle<void> coro) {
        // finish the serving ...
        link.Finish(status, coro.address());
    });
    co_return status;
}

void serve_queue(CompletionQueue& cq, Server& s) noexcept(false);

void serve(grpc_impl::Server& s, //
           grpc_impl::ServerCompletionQueue& queue,
           v1::Worker::AsyncService& service) noexcept(false) {
    // we have 4 method. each will be served with coroutines
    // all coroutines will be destructed automatically
    serve_method1(queue, service);
    serve_method2(queue, service);
    serve_method3(queue, service);
    serve_method4(queue, service);

    // run with the completion queue.
    // the grpc server will be shutdowned when a signal raised
    return serve_queue(queue, s);
}
