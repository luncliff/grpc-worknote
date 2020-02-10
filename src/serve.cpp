#include <gsl/gsl>
#include <new>

#include <grpcpp/grpcpp.h>
#include <service.grpc.pb.h>

#if __has_include(<coroutine>)
#include <coroutine>
using namespace std;

#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
using namespace std;
using namespace std::experimental;

#endif

using rpc_callback_t = function<void(coroutine_handle<void>)>;

class rpc_context_t {
  protected:
    rpc_callback_t callback{};

  public:
    void set_failed() noexcept {
        callback = nullptr;
    }

  public:
    constexpr bool await_ready() noexcept {
        return false;
    }
    void await_suspend(coroutine_handle<void> coro) noexcept(false) {
        return callback(coro);
    }
    // cancelled?
    bool await_resume() noexcept {
        return static_cast<bool>(callback); // == nullptr ?
    }
};

class rpc_routine_t final {
  public:
    class promise_type final : public rpc_context_t {
      public:
        suspend_never initial_suspend() noexcept {
            return {};
        }
        suspend_never final_suspend() noexcept {
            return {};
        }
        void unhandled_exception() noexcept(false) {
            throw;
        }
        auto get_return_object() noexcept {
            return rpc_routine_t{this};
        }
        void return_void() noexcept {
        }
        auto await_transform(rpc_callback_t&& fn) noexcept -> rpc_context_t& {
            this->callback = move(fn);
            return *this;
        }
    };

  public:
    explicit rpc_routine_t(promise_type*) noexcept(false){};
};

void serve_user_data(void* user_data, bool ok) noexcept(false) {
    using promise_t = rpc_routine_t::promise_type;
    auto task = coroutine_handle<promise_t>::from_address(user_data);
    if (ok == false)
        task.promise().set_failed();

    if (task.done() == false)
        task.resume();
}

using namespace grpc;

using grpc_server_queue_t = grpc_impl::ServerCompletionQueue;
auto serve_method1(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t;
auto serve_method2(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t;
auto serve_method3(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t;
auto serve_method4(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t;

void serve_queue(grpc::CompletionQueue& cq, grpc::Server& s) noexcept(false);
void serve(grpc_impl::Server& s, //
           grpc_impl::ServerCompletionQueue& queue) noexcept(false) {

    serve_method1(queue); // we have 4 method in 'service.proto'.
    serve_method2(queue); // each will be served with coroutines
    serve_method3(queue);
    serve_method4(queue);

    // the grpc server will be shutdowned when a signal raised
    return serve_queue(queue, s);
}

v1::Worker::AsyncService service{};

void register_impl(grpc::ServerBuilder& builder) noexcept(false) {
    // register service and start with completion queue
    builder.RegisterService(&service);
}

auto serve_request(v1::Request& req, v1::Response& res) -> grpc::Status {
    return grpc::Status::OK;
}

bool is_shutdowned() noexcept;

auto serve_method1(grpc_server_queue_t& queue) noexcept(false)
    -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method1(queue);
    });

    ServerContext ctx{};
    ServerAsyncResponseWriter<v1::Response> responder{&ctx};
    v1::Request req{};

    auto ok = co_await [&](coroutine_handle<void> coro) {
        service.RequestMethod1(&ctx, &req, &responder, &queue, &queue,
                               coro.address());
    };
    if (ok == false)
        co_return;

    v1::Response res{};
    grpc::Status status = serve_request(req, res);

    co_await [&](coroutine_handle<void> coro) {
        responder.Finish(res, status, coro.address());
    };
    co_return;
}

auto serve_method2(grpc_server_queue_t& queue) noexcept(false)
    -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method2(queue);
    });

    ServerContext ctx{};
    ServerAsyncWriter<v1::Response> writer{&ctx};
    v1::Request req{};

    co_await [&](coroutine_handle<void> coro) {
        service.RequestMethod2(&ctx, &req, &writer, &queue, &queue,
                               coro.address());
    };

    v1::Response res{};
    grpc::Status status = serve_request(req, res);

    auto repeat = 1u;
    while (repeat--) {
        co_await [&](coroutine_handle<void> coro) {
            writer.Write(res, coro.address());
        };
    }

    co_await [&](coroutine_handle<void> coro) {
        writer.Finish(status, coro.address());
    };
}

auto serve_method3(grpc_server_queue_t& queue) noexcept(false)
    -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method3(queue);
    });

    ServerContext ctx{};
    ServerAsyncReader<v1::Response, v1::Request> reader{&ctx};

    co_await [&](coroutine_handle<void> coro) {
        service.RequestMethod3(&ctx, &reader, &queue, &queue, coro.address());
    };

    v1::Request req{};
    v1::Response res{};
    grpc::Status status{};

    while (co_await [&](coroutine_handle<void> coro) {
        reader.Read(&req, coro.address());
    }) {
        // will return the last one
        status = serve_request(req, res);
    }
    co_await [&](coroutine_handle<void> coro) {
        reader.Finish(res, status, coro.address());
    };
}

auto serve_method4(grpc_server_queue_t& queue) noexcept(false)
    -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method3(queue);
    });

    ServerContext ctx{};
    ServerAsyncReaderWriter<v1::Response, v1::Request> session{&ctx};

    co_await [&](coroutine_handle<void> coro) {
        service.RequestMethod4(&ctx, &session, &queue, &queue, coro.address());
    };

    v1::Request req{};
    v1::Response res{};
    grpc::Status status{};

    // response for all successful read ...
    while (co_await [&](coroutine_handle<void> coro) {
        session.Read(&req, coro.address());
    }) {
        status = serve_request(req, res);
        co_await [&](coroutine_handle<void> coro) {
            session.Write(res, coro.address());
        };
    }

    co_await [&](coroutine_handle<void> coro) {
        // finish the serving ...
        session.Finish(status, coro.address());
    };
}
