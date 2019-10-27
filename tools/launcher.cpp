
#include <cstdlib>
#include <grpcpp/grpcpp.h>

#include "loader.h"
#include "plugin_core.h"
#include "service.grpc.pb.h"

void serve(grpc::Server& launcher, grpc_impl::ServerCompletionQueue& queue,
           plugins_v1::Executor1::AsyncService& service) noexcept(false);

plugin_t make_plugin(loader_t& dll) noexcept(false);

std::unique_ptr<plugin_t> plugin_uptr{};

const plugin_t& get_current_plugin() noexcept {
    return *plugin_uptr;
}

// "launcher.exe" "module.dll" "0.0.0.0:9090" "service.pem" "service.key"
int main(int argc, char* argv[]) try {
    auto loader = loader_t::load(argv[1]);
    plugin_uptr = std::make_unique<plugin_t>(make_plugin(loader));

    grpc::SslServerCredentialsOptions tls{};
    grpc::ServerBuilder builder{};

    auto creds = grpc::InsecureServerCredentials();
    if (argc < 3) {
        puts("expects address in argv[2] \n");
        return EXIT_FAILURE;
    }
    if (argc > 4) {
        puts("using TLS \n");
        tls.pem_key_cert_pairs.push_back({argv[4], argv[3]});
        creds = grpc::SslServerCredentials(tls);
    }
    builder.AddListeningPort(argv[2], creds);

    plugins_v1::Executor1::AsyncService service{};
    builder.RegisterService(&service);

    auto queue = builder.AddCompletionQueue();
    auto launcher = builder.BuildAndStart();

    serve(*launcher, *queue, service);
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

template <typename FnType>
auto verbose_lookup(loader_t& dll, gsl::czstring<> label) {
    auto fn = dll.template lookup<FnType>(label);
    if (fn != nullptr)
        printf("%s: %p\n", label, fn);
    return fn;
}

plugin_t make_plugin(loader_t& dll) noexcept(false) {

    auto fcreate = verbose_lookup<plugin_create_t>(dll, "on_create");
    auto fdestroy = verbose_lookup<plugin_destroy_t>(dll, "on_destroy");
    auto fready = verbose_lookup<plugin_status_t>(dll, "on_ready");
    auto fidle = verbose_lookup<plugin_status_t>(dll, "on_idle");

    auto fserve1 = verbose_lookup<plugin_serve_t>(dll, "on_serve_1");
    auto fserve2 = verbose_lookup<plugin_serve_t>(dll, "on_serve_2");
    auto fserve3 = verbose_lookup<plugin_serve_t>(dll, "on_serve_3");
    auto fserve4 = verbose_lookup<plugin_serve_t>(dll, "on_serve_4");

    return plugin_t{
        .on_create = gsl::make_not_null(fcreate),
        .on_destroy = gsl::make_not_null(fdestroy),
        .on_ready = gsl::make_not_null(fready),
        .on_idle = gsl::make_not_null(fidle),
        .on_serve_1 = gsl::make_not_null(fserve1),
        .on_serve_2 = gsl::make_not_null(fserve2),
        .on_serve_3 = gsl::make_not_null(fserve3),
        .on_serve_4 = gsl::make_not_null(fserve4),
    };
}
