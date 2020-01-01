
#include <system_error>

#include <grpcpp/grpcpp.h>
#include <service.grpc.pb.h>

using namespace std;

void serve(grpc_impl::Server& s, //
           grpc_impl::ServerCompletionQueue& queue,
           v1::Worker::AsyncService& service) noexcept(false);

grpc::SslServerCredentialsOptions tls{};

int on_exception(exception_ptr e = current_exception()) noexcept;

// service.exe 0.0.0.0:9090 server.key server.crt
int main(int argc, char* argv[]) try {
    auto creds = grpc::InsecureServerCredentials();
    if (argc < 2)
        throw runtime_error{"expects address in argv[1]"};
    if (argc >= 4) {
        cerr << "key " << argv[2] << endl;
        cerr << "cert " << argv[3] << endl;
        tls.pem_key_cert_pairs.push_back({argv[2], argv[3]});
        creds = grpc::SslServerCredentials(tls);
    }

    grpc::ServerBuilder builder{};
    builder.AddListeningPort(argv[1], creds);

    // register service and start with completion queue
    v1::Worker::AsyncService service{};
    builder.RegisterService(&service);

    auto queue = builder.AddCompletionQueue();
    auto server = builder.BuildAndStart();
    cout << "serving... " << argv[1] << endl;
    serve(*server, *queue, service);
    return EXIT_SUCCESS;
} catch (...) {
    return on_exception();
}

int on_exception(exception_ptr e) noexcept {
    try {
        rethrow_exception(e);
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
