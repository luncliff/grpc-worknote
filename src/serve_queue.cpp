#include <chrono>
#include <cstdio>
#include <gsl/gsl>

#include <grpcpp/grpcpp.h>

void serve_user_data(void* user_data, bool ok) noexcept(false);

// fetch and resume without signal checking
void drain_queue(grpc::CompletionQueue& cq) noexcept(false) {
    void* user_data = nullptr;
    bool ok = false;
    while (cq.Next(&user_data, &ok))
        serve_user_data(user_data, ok);
}

bool drain_queue_once(
    grpc::CompletionQueue& cq,
    std::chrono::system_clock::time_point deadline) noexcept(false) {
    void* user_data = nullptr;
    bool ok = false;

    const auto status = cq.AsyncNext(&user_data, &ok, deadline);
    switch (status) {
    case grpc_impl::CompletionQueue::NextStatus::SHUTDOWN:
        return true;
    case grpc_impl::CompletionQueue::NextStatus::GOT_EVENT:
        serve_user_data(user_data, ok);
    case grpc_impl::CompletionQueue::NextStatus::TIMEOUT:
    default:
        return false;
    }
}

bool is_signaled() noexcept;
void watch_signals() noexcept(false);

bool is_shutdowned() noexcept {
    return is_signaled();
}

void serve_queue(grpc::CompletionQueue& cq, grpc::Server& s) noexcept(false) {
    auto on_return = gsl::finally([&]() {
        // no more request
        // https://grpc.github.io/grpc/cpp/classgrpc_1_1_server_interface.html#a644772d11101318f1424e595dee73ccf
        s.Shutdown();
        // after shutdown, we have to ensure the queue is drained
        // see https://github.com/grpc/grpc/issues/8610
        cq.Shutdown();
        return drain_queue(cq);
    });

    using namespace std::chrono;
    using namespace std::chrono_literals;

    // fetch(with timeout) and resume with signal checking
    watch_signals();
    while (is_signaled() == false)
        if (auto shutdowned = drain_queue_once(cq, system_clock::now() + 1s))
            break;
}
