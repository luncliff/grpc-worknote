
#include "../src/plugin_impl.h"
#include <service.grpc.pb.h>

#include "grpc_coroutine.h"

using namespace grpc;

auto until(function<void(coroutine_handle<void>)>&& on_suspend) {
    return on_suspend;
}

auto handle_invoke1(grpc_impl::ServerCompletionQueue& queue,
                    plugins::Executor1::AsyncService& service) noexcept(false)
    -> service_coroutine_t {
    ServerContext sctx{};
    ServerAsyncResponseWriter<plugins::Response1> responder{&sctx};

    plugin_request_t req{};

    co_await until([&](coroutine_handle<void> coro) {
        service.RequestInvoke1(&sctx, &req, &responder, &queue, &queue,
                               coro.address());
    });

    plugin_response_t res{};
    grpc::Status status{};

    co_await until([&](coroutine_handle<void> coro) {
        responder.Finish(res, status, coro.address());
    });
    co_return status;
}

auto handle_invoke2(grpc_impl::ServerCompletionQueue& queue,
                    plugins::Executor1::AsyncService& service) noexcept(false)
    -> service_coroutine_t {
    ServerContext sctx{};
    ServerAsyncWriter<plugins::Response1> writer{&sctx};

    plugin_request_t req{};

    co_await until([&](coroutine_handle<void> coro) {
        service.RequestInvoke2(&sctx, &req, &writer, &queue, &queue,
                               coro.address());
    });

    plugin_response_t res{};
    grpc::Status status{};

    for (auto i = 0u; i < 1u; ++i)
        co_await until([&](coroutine_handle<void> coro) {
            writer.Write(res, coro.address());
        });
    co_await until([&](coroutine_handle<void> coro) {
        writer.Finish(status, coro.address());
    });
    co_return status;
}

auto handle_invoke3(grpc_impl::ServerCompletionQueue& queue,
                    plugins::Executor1::AsyncService& service) noexcept(false)
    -> service_coroutine_t {
    ServerContext sctx{};
    ServerAsyncReader<plugins::Response1, plugins::Request1> reader{&sctx};

    co_await until([&](coroutine_handle<void> coro) {
        service.RequestInvoke3(&sctx, &reader, &queue, &queue, coro.address());
    });

    plugin_request_t req{};
    plugin_response_t res{};
    grpc::Status status{};

    while (co_await until([&](coroutine_handle<void> coro) {
        reader.Read(&req, coro.address());
    })) {
        // on read ...
    }

    co_await until([&](coroutine_handle<void> coro) {
        reader.Finish(res, status, coro.address());
    });
    co_return status;
}

auto handle_invoke4(grpc_impl::ServerCompletionQueue& queue,
                    plugins::Executor1::AsyncService& service) noexcept(false)
    -> service_coroutine_t {
    grpc::ServerContext sctx{};
    ServerAsyncReaderWriter<plugins::Response1, plugins::Request1> link{&sctx};

    co_await until([&](coroutine_handle<void> coro) {
        service.RequestInvoke4(&sctx, &link, &queue, &queue, coro.address());
    });

    plugin_request_t req{};
    plugin_response_t res{};
    grpc::Status status{};

    co_await until([&](coroutine_handle<void> coro) {
        // read one time ...
        link.Read(&req, coro.address());
    });
    co_await until([&](coroutine_handle<void> coro) {
        // write one time ...
        link.Write(res, coro.address());
    });
    co_await until([&](coroutine_handle<void> coro) {
        // finish the serving ...
        link.Finish(status, coro.address());
    });
    co_return status;
}

class plugin_service_t final : public plugins::Executor1::AsyncService {
    plugin_t& p;

  public:
    plugin_service_t(plugin_t& _plugin) noexcept
        : plugins::Executor1::AsyncService{}, p{_plugin} {
    }

  public:
    Status Invoke1(ServerContext* context, const plugins::Request1* request,
                   plugins::Response1* response) override {
        return Status(grpc::StatusCode::OK, "");
    }

    Status Invoke2(ServerContext* context, const plugins::Request1* request,
                   ServerWriter<plugins::Response1>* writer) override {
        return Status(StatusCode::OK, "");
    }

    Status Invoke3(ServerContext* context,
                   ServerReader<plugins::Request1>* reader,
                   plugins::Response1* response) override {
        return Status(StatusCode::OK, "");
    }

    Status Invoke4(ServerContext* context,
                   ServerReaderWriter<plugins::Response1, plugins::Request1>*
                       stream) override {
        return Status(StatusCode::OK, "");
    }
};

auto make_service(plugin_t& p) noexcept(false)
    -> std::unique_ptr<plugins::Executor1::AsyncService> {
    return make_unique<plugin_service_t>(p);
}
