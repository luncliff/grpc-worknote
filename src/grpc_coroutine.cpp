
#include "grpc_coroutine.h"
#include "grpc_plugin.h"

using namespace std;

auto until(function<void(coroutine_handle<void>)>&& on_suspend) {
    return on_suspend;
}

auto make_request() noexcept(false) -> unique_ptr<plugin_request_t>;
auto make_response() noexcept(false) -> unique_ptr<plugin_response_t>;
auto translate(plugin_error_t ec) noexcept -> grpc::Status {
    return {};
}

auto serve_with_plugin(plugins::Executor1::AsyncService& service,
                       grpc::ServerCompletionQueue& cq, plugin_t& p,
                       gsl::not_null<plugin_context_t*> pctx)
    -> grpc_service_routine_t {
    grpc::ServerContext ctx{};
    grpc::ServerAsyncResponseWriter<plugins::Response1> responder{&ctx};
    grpc::Status status{};

    auto on_return = gsl::finally([c = pctx.get(), &p]() { p.on_idle(c); });
    auto req = make_request();
    auto res = make_response();

    for (p.on_ready(pctx.get());
         // read a request
         co_await until([&, ptr = req.get()](coroutine_handle<void> coro) {
             service.RequestInvoke1(&ctx, ptr, &responder, &cq, &cq,
                                    coro.address());
         });
         // send a response
         co_await until([&, ptr = res.get()](coroutine_handle<void> coro) {
             responder.Finish(*ptr, status, coro.address());
         })) {
        // serve a request
        const auto ec = p.on_request_1(pctx.get(), *req, *res);
        status = translate(ec);
    }
    co_return status;
}

auto serve(plugins::Executor1::AsyncService& service,
           grpc::ServerCompletionQueue& cq, plugin_t& p) noexcept(false) {
    const auto attr = make_default_attr();
    auto ctx = unique_ptr<plugin_context_t, plugin_destroy_t>{
        p.on_create(attr), p.on_destroy.get()};
    if (ctx == nullptr)
        return;

    serve_with_plugin(service, cq, p, ctx.get());
}
