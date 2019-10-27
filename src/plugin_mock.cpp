
#include "plugin_impl.h"

struct mock_context_t {};

extern "C" {

auto on_create(const plugin_attr_t&) -> plugin_context_t* {
    auto ptr = new (std::nothrow) mock_context_t{};
    fprintf(stderr, "%s: %p\n", __FUNCTION__, ptr);

    return reinterpret_cast<plugin_context_t*>(ptr);
}
void on_destroy(plugin_context_t* ctx) {
    auto ptr = reinterpret_cast<mock_context_t*>(ctx);
    fprintf(stderr, "%s: %p\n", __FUNCTION__, ptr);

    delete ptr;
}

void on_ready(gsl::not_null<plugin_context_t*> ctx) {
    fprintf(stderr, "%s: %p\n", __FUNCTION__, ctx.get());
}
void on_idle(gsl::not_null<plugin_context_t*> ctx) {
    fprintf(stderr, "%s: %p\n", __FUNCTION__, ctx.get());
}

auto on_serve(gsl::not_null<plugin_context_t*> ctx, //
              const plugin_request_t& req, plugin_response_t& res, bool& ready,
              gsl::czstring<> caller_name) -> plugin_error_t {
    auto blob = get_blob(req);
    res.set_blob(blob.data(), blob.size_bytes());

    ready = fprintf(stderr, "%s: %p %zu\n", caller_name, ctx.get(),
                    blob.size_bytes()) > 0;
    return 0;
}
auto on_serve_1(gsl::not_null<plugin_context_t*> ctx, //
                const plugin_request_t& req, plugin_response_t& res,
                bool& ready) -> plugin_error_t {
    return on_serve(ctx, req, res, ready, __FUNCTION__);
}

auto on_serve_2(gsl::not_null<plugin_context_t*> ctx, //
                const plugin_request_t& req, plugin_response_t& res,
                bool& ready) -> plugin_error_t {
    return on_serve(ctx, req, res, ready, __FUNCTION__);
}
auto on_serve_3(gsl::not_null<plugin_context_t*> ctx, //
                const plugin_request_t& req, plugin_response_t& res,
                bool& ready) -> plugin_error_t {
    return on_serve(ctx, req, res, ready, __FUNCTION__);
}
auto on_serve_4(gsl::not_null<plugin_context_t*> ctx, //
                const plugin_request_t& req, plugin_response_t& res,
                bool& ready) -> plugin_error_t {
    return on_serve(ctx, req, res, ready, __FUNCTION__);
}
}
