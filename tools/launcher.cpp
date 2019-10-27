
#include <plugin_core.h>

#include <grpcpp/grpcpp.h>

#include "loader.h"
#include "service.grpc.pb.h"

void serve(grpc::Server& launcher, grpc_impl::ServerCompletionQueue& queue,
           plugins_v1::Executor1::AsyncService& service,
           plugin_t& plugin) noexcept(false);

plugin_t make_plugin(loader_t& dll) noexcept(false);

// "launcher.exe" "module.dll" "0.0.0.0:10443"
int main(int argc, char* argv[]) try {

    auto loader = loader_t::load(argv[1]);
    auto plugin = make_plugin(loader);

    grpc::ServerBuilder builder{};
    auto creds = grpc::InsecureServerCredentials();
    builder.AddListeningPort(argv[2], creds);

    plugins_v1::Executor1::AsyncService service{};
    builder.RegisterService(&service);

    auto queue = builder.AddCompletionQueue();
    auto launcher = builder.BuildAndStart();

    serve(*launcher, *queue, service, plugin);
    return EXIT_SUCCESS;

} catch (const std::error_code& ec) {
    auto msg = ec.message();
    fputs(msg.c_str(), stderr);
    return ec.value();

} catch (const std::system_error& ex) {
    fputs(ex.what(), stderr);
    return ex.code().value();

} catch (const std::exception& ex) {
    fputs(ex.what(), stderr);
    return EXIT_FAILURE;
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
