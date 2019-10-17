
#include <plugin_core.h>
#include <service.grpc.pb.h>

#include "grpc_coroutine.h"
#include "loader.h"

plugin_t make_plugin(loader_t& dll) noexcept(false);
auto make_service(plugin_t& p) noexcept(false)
    -> std::unique_ptr<plugins::Executor1::AsyncService>;

uint32_t serve(grpc::Server& launcher, grpc_impl::ServerCompletionQueue& queue,
               plugins::Executor1::AsyncService& service) noexcept;

int main(int argc, char* argv[]) {
    auto loader = loader_t::load(argv[2]);
    auto plugin = make_plugin(loader);

    grpc::ServerBuilder builder{};
    builder.AddListeningPort(argv[1], grpc::InsecureServerCredentials());

    auto service = make_service(plugin);
    builder.RegisterService(service.get());

    auto cq = builder.AddCompletionQueue();
    auto launcher = builder.BuildAndStart();
    return serve(*launcher, *cq, *service);
}

plugin_t make_plugin(loader_t& dll) noexcept(false) {

    plugin_create_t fcreate = dll.lookup<plugin_create_t>("on_create");
    plugin_destroy_t fdestroy = dll.lookup<plugin_destroy_t>("on_destroy");
    plugin_status_t fready = dll.lookup<plugin_status_t>("on_ready");
    plugin_status_t fidle = dll.lookup<plugin_status_t>("on_idel");
    plugin_serve_t fserve1 = dll.lookup<plugin_serve_t>("on_request_1");

    return plugin_t{
        .on_create = gsl::make_not_null(fcreate),
        .on_destroy = gsl::make_not_null(fdestroy),
        .on_ready = gsl::make_not_null(fready),
        .on_idle = gsl::make_not_null(fidle),
        .on_request_1 = gsl::make_not_null(fserve1),
    };
}
