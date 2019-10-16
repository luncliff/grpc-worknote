
#include "grpc_plugin.h"

using namespace std;

// todo: replace to arena allocator
auto make_request() noexcept(false) -> unique_ptr<plugin_request_t> {
    auto req = make_unique<plugin_request_t>();
    return req;
}

// todo: replace to arena allocator
auto make_response() noexcept(false) -> unique_ptr<plugin_response_t> {
    auto res = make_unique<plugin_response_t>();
    return res;
}

/*
auto run_with_plugin(plugin_t& p, const plugin_attr_t& attr) noexcept(false) {
    auto ctx = unique_ptr<plugin_context_t, plugin_destroy_t>{
        p.on_create(attr), p.on_destroy.get()};
    if (ctx == nullptr)
        return;
    auto on_return = gsl::finally([c = ctx.get(), &p]() { p.on_idle(c); });
    auto req = make_request();
    auto res = make_response();
    for (p.on_ready(ctx.get()); read_request(*req); send_response(*res))
        p.on_request_1(ctx.get(), *req, *res);
}
*/