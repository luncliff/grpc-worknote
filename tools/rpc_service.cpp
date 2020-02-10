/**
 * @file rpc_service.cpp
 * @author luncliff (luncliff@gmail.com)
 * @brief Entrypoint of the service application
 * @details
 * The program uses positional argument.
 * Each position's usage are like the following.
 * 1. program name
 * 2. local listening address (`argv[1]`)
 * 3. /path/to/server.key (`argv[2]`)
 * 4. /path/to/server.crt (`argv[3]`)
 *
 * ```console
 * $ ./rpc_service 0.0.0.0:9090 server.key server.crt
 * ```
 */
#include <grpcpp/grpcpp.h>
#include <system_error>

extern void register_impl(grpc::ServerBuilder& builder) noexcept(false);
extern void serve(grpc_impl::Server& s, //
                  grpc_impl::ServerCompletionQueue& queue) noexcept(false);

int main(int argc, char* argv[]) {
    using namespace std;
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
