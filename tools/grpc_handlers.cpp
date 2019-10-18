
#include "../src/plugin_impl.h"
#include <service.grpc.pb.h>

#include "grpc_shared.h"

using namespace std;
using namespace grpc;

auto until(function<void(coroutine_handle<void>)>&& on_suspend) noexcept {
    return on_suspend;
}

auto serve_invoke1(grpc_impl::ServerCompletionQueue& queue,
                   plugins::Executor1::AsyncService& service,
                   gsl::not_null<plugin_context_t*> pctx,
                   plugin_t& plugin) noexcept(false) -> service_coroutine_t {

    // serve next request unless the launcher is shutdowned
    auto on_return = gsl::finally([&, ctx = pctx.get()]() {
        if (is_shutdowned() == false)
            serve_invoke1(queue, service, ctx, plugin);
    });

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

auto serve_invoke2(grpc_impl::ServerCompletionQueue& queue,
                   plugins::Executor1::AsyncService& service,
                   gsl::not_null<plugin_context_t*> pctx,
                   plugin_t& plugin) noexcept(false) -> service_coroutine_t {

    // serve next request unless the launcher is shutdowned
    auto on_return = gsl::finally([&, ctx = pctx.get()]() {
        if (is_shutdowned() == false)
            serve_invoke1(queue, service, ctx, plugin);
    });

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

auto serve_invoke3(grpc_impl::ServerCompletionQueue& queue,
                   plugins::Executor1::AsyncService& service,
                   gsl::not_null<plugin_context_t*> pctx,
                   plugin_t& plugin) noexcept(false) -> service_coroutine_t {

    // serve next request unless the launcher is shutdowned
    auto on_return = gsl::finally([&, ctx = pctx.get()]() {
        if (is_shutdowned() == false)
            serve_invoke1(queue, service, ctx, plugin);
    });

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

auto serve_invoke4(grpc_impl::ServerCompletionQueue& queue,
                   plugins::Executor1::AsyncService& service,
                   gsl::not_null<plugin_context_t*> pctx,
                   plugin_t& plugin) noexcept(false) -> service_coroutine_t {

    // serve next request unless the launcher is shutdowned
    auto on_return = gsl::finally([&, ctx = pctx.get()]() {
        if (is_shutdowned() == false)
            serve_invoke1(queue, service, ctx, plugin);
    });

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
