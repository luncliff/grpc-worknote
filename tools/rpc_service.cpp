
#include <system_error>

#include <grpcpp/grpcpp.h>

using namespace std;

extern void register_impl(grpc::ServerBuilder& builder) noexcept(false);
extern void serve(grpc_impl::Server& s, //
                  grpc_impl::ServerCompletionQueue& queue) noexcept(false);

// service.exe 0.0.0.0:9090 server.key server.crt
int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw runtime_error{"expects address in argv[1]"};

        auto creds = grpc::InsecureServerCredentials();
        if (argc >= 4) {
            grpc::SslServerCredentialsOptions tls{};
            tls.pem_key_cert_pairs.push_back({argv[2], argv[3]});
            creds = grpc::SslServerCredentials(tls);
        }

        grpc::ServerBuilder builder{};
        builder.AddListeningPort(argv[1], creds);

        // register service and start with completion queue
        register_impl(builder);
        auto queue = builder.AddCompletionQueue();
        auto server = builder.BuildAndStart();
        serve(*server, *queue);

        return EXIT_SUCCESS;
    } catch (const system_error& ex) {
        cerr << ex.what() << endl;
        return ex.code().value();
    } catch (const error_code& ex) {
        cerr << ex.message() << endl;
        return ex.value();
    } catch (const exception& ex) {
        cerr << ex.what() << endl;
    }
    return EXIT_FAILURE;
}
