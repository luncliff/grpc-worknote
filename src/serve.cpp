/**
 * @file serve.cpp
 * @author luncliff (luncliff@gmail.com)
 * @brief Server methods' implementation
 */
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

/**
 * @brief Callback to pass `coroutine_handle<void>` to gRPC completion queue
 * @details In this implementation,
 * `coroutine_handle<void>` becomes an user data for gRPC completion queue
 *
 * @see std::function<T>
 */
using rpc_callback_t = function<void(coroutine_handle<void>)>;

/**
 * @brief User data for gRPC completion queue in this implementation
 */
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
        /** @brief never suspend for the coroutine's initial suspend */
        suspend_never initial_suspend() noexcept {
            return {};
        }
        /** @brief never suspend for the coroutine's final suspend */
        suspend_never final_suspend() noexcept {
            return {};
        }
        void unhandled_exception() noexcept(false) {
            throw;
        }
        auto get_return_object() noexcept {
            return rpc_routine_t{this};
        }
        /**
         * @brief `co_return` statement won't take argument
         */
        void return_void() noexcept {
        }

        /**
         * @brief Changes `rpc_callback_t` to `rpc_context_t`
         * @details Save the given functor to current context
         * @see rpc_callback_t
         */
        auto await_transform(rpc_callback_t&& fn) noexcept -> rpc_context_t& {
            this->callback = move(fn);
            return *this;
        }
    };

  public:
    explicit rpc_routine_t(promise_type*) noexcept(false){};
};

/**
 * @brief Recognize the `user_data` and resumes the task
 *
 * @param user_data
 * @param ok        if `false`, the operation is failed (e.g. cancelled)
 */
void serve_user_data(void* user_data, bool ok) noexcept(false) {
    using promise_t = rpc_routine_t::promise_type;
    auto task = coroutine_handle<promise_t>::from_address(user_data);
    if (ok == false)
        task.promise().set_failed();

    if (task.done() == false)
        task.resume();
}

using namespace grpc;

// alias to shorten the code
using grpc_server_queue_t = grpc_impl::ServerCompletionQueue;
auto serve_method1(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t;
auto serve_method2(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t;
auto serve_method3(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t;
auto serve_method4(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t;

void serve_queue(grpc::CompletionQueue& cq, grpc::Server& s) noexcept(false);

/**
 * @brief Prepare to handle the methods and start serving with the queue
 * @param s
 * @param queue
 * @see 'service.proto'
 * @details
 *
 * * The grpc server will be shutdowned when a signal is raised
 *
 */
void serve(grpc_impl::Server& s, //
           grpc_impl::ServerCompletionQueue& queue) noexcept(false) {

    serve_method1(queue); // we have 4 method in 'service.proto'.
    serve_method2(queue); // each will be served with coroutines
    serve_method3(queue);
    serve_method4(queue);

    // the grpc server will be shutdowned when a signal raised
    return serve_queue(queue, s);
}

/**
 * @brief singleton instance of the `v1::Worker` service
 */
v1::Worker::AsyncService service1{};

/**
 * @brief Register internal service implementation to given `ServerBuilder`
 * @details The function registers service but won't trigger start/creation
 * @param builder
 */
void register_impl(grpc::ServerBuilder& builder) noexcept(false) {
    builder.RegisterService(&service1);
}

/**
 * @brief Mock implementation for request handler
 *
 * @param req
 * @param res
 * @return grpc::Status
 */
auto serve_request(v1::Request& req, v1::Response& res) -> grpc::Status {
    return grpc::Status::OK;
}

bool is_shutdowned() noexcept;

/**
 * @param queue
 * @return rpc_routine_t
 * @see service.proto
 */
auto serve_method1(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method1(queue);
    });

    ServerContext ctx{};
    ServerAsyncResponseWriter<v1::Response> responder{&ctx};
    v1::Request req{};

    auto ok = co_await [&](coroutine_handle<void> coro) {
        service1.RequestMethod1(&ctx, &req, &responder, &queue, &queue, coro.address());
    };
    if (ok == false)
        co_return;

    v1::Response res{};
    grpc::Status status = serve_request(req, res);

    co_await [&](coroutine_handle<void> coro) { responder.Finish(res, status, coro.address()); };
    co_return;
}

/**
 * @param queue
 * @return rpc_routine_t
 * @see service.proto
 */
auto serve_method2(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method2(queue);
    });

    ServerContext ctx{};
    ServerAsyncWriter<v1::Response> writer{&ctx};
    v1::Request req{};

    co_await [&](coroutine_handle<void> coro) {
        service1.RequestMethod2(&ctx, &req, &writer, &queue, &queue, coro.address());
    };

    v1::Response res{};
    grpc::Status status = serve_request(req, res);

    auto repeat = 1u;
    while (repeat--) {
        co_await [&](coroutine_handle<void> coro) { writer.Write(res, coro.address()); };
    }

    co_await [&](coroutine_handle<void> coro) { writer.Finish(status, coro.address()); };
}

/**
 * @param queue
 * @return rpc_routine_t
 * @see service.proto
 */
auto serve_method3(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method3(queue);
    });

    ServerContext ctx{};
    ServerAsyncReader<v1::Response, v1::Request> reader{&ctx};

    co_await
        [&](coroutine_handle<void> coro) { service1.RequestMethod3(&ctx, &reader, &queue, &queue, coro.address()); };

    v1::Request req{};
    v1::Response res{};
    grpc::Status status{};

    while (co_await [&](coroutine_handle<void> coro) { reader.Read(&req, coro.address()); }) {
        // will return the last one
        status = serve_request(req, res);
    }
    co_await [&](coroutine_handle<void> coro) { reader.Finish(res, status, coro.address()); };
}

/**
 * @param queue
 * @return rpc_routine_t
 * @see service.proto
 */
auto serve_method4(grpc_server_queue_t& queue) noexcept(false) -> rpc_routine_t {
    auto on_return = gsl::finally([&]() {
        if (is_shutdowned() == false)
            serve_method3(queue);
    });

    ServerContext ctx{};
    ServerAsyncReaderWriter<v1::Response, v1::Request> session{&ctx};

    co_await
        [&](coroutine_handle<void> coro) { service1.RequestMethod4(&ctx, &session, &queue, &queue, coro.address()); };

    v1::Request req{};
    v1::Response res{};
    grpc::Status status{};

    // response for all successful read ...
    while (co_await [&](coroutine_handle<void> coro) { session.Read(&req, coro.address()); }) {
        status = serve_request(req, res);
        co_await [&](coroutine_handle<void> coro) { session.Write(res, coro.address()); };
    }

    co_await [&](coroutine_handle<void> coro) {
        // finish the serving ...
        session.Finish(status, coro.address());
    };
}
