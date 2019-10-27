
#include "grpc_shared.h"
#include "plugin_core.h"
#include "service.grpc.pb.h"

using namespace std;

auto serve_invoke1(grpc_impl::ServerCompletionQueue& queue,
                   plugins_v1::Executor1::AsyncService& service,
                   gsl::not_null<plugin_context_t*> pctx,
                   plugin_t& plugin) noexcept(false) -> service_coroutine_t;

auto serve_invoke2(grpc_impl::ServerCompletionQueue& queue,
                   plugins_v1::Executor1::AsyncService& service,
                   gsl::not_null<plugin_context_t*> pctx,
                   plugin_t& plugin) noexcept(false) -> service_coroutine_t;

auto serve_invoke3(grpc_impl::ServerCompletionQueue& queue,
                   plugins_v1::Executor1::AsyncService& service,
                   gsl::not_null<plugin_context_t*> pctx,
                   plugin_t& plugin) noexcept(false) -> service_coroutine_t;

auto serve_invoke4(grpc_impl::ServerCompletionQueue& queue,
                   plugins_v1::Executor1::AsyncService& service,
                   gsl::not_null<plugin_context_t*> pctx,
                   plugin_t& plugin) noexcept(false) -> service_coroutine_t;

auto make_plugin_context(plugin_t& plugin, plugin_attr_t& attr) {
    return unique_ptr<plugin_context_t, plugin_destroy_t>{
        plugin.on_create(attr), plugin.on_destroy};
}

void consume_queue(grpc::Server& s, grpc::CompletionQueue& q) noexcept(false);

void serve(grpc::Server& launcher, grpc_impl::ServerCompletionQueue& queue,
           plugins_v1::Executor1::AsyncService& service,
           plugin_t& plugin) noexcept(false) {

    // we have 4 method. each will be served with coroutines
    // all coroutines will be destructed automatically
    auto attr = make_default_attr();

    // each coroutine will hold a context from the plugin

    auto ctx1 = make_plugin_context(plugin, attr);
    if (ctx1)
        serve_invoke1(queue, service, ctx1.get(), plugin);

    auto ctx2 = make_plugin_context(plugin, attr);
    if (ctx2)
        serve_invoke2(queue, service, ctx2.get(), plugin);

    auto ctx3 = make_plugin_context(plugin, attr);
    if (ctx3)
        serve_invoke3(queue, service, ctx3.get(), plugin);

    auto ctx4 = make_plugin_context(plugin, attr);
    if (ctx4)
        serve_invoke4(queue, service, ctx4.get(), plugin);

    // run with the completion queue.
    // the grpc server will be shutdowned when a signal raised
    return consume_queue(launcher, queue);
}
