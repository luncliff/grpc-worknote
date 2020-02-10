/**
 * @file serve_queue.cpp
 * @author luncliff (luncliff@gmail.com)
 * @brief gRPC completion queue related code
 */
#include <chrono>
#include <cstdio>
#include <gsl/gsl>

#include <grpcpp/grpcpp.h>

void serve_user_data(void* user_data, bool ok) noexcept(false);

/**
 * @brief Fetch and resume queued work
 *
 * @param cq    gRPC completion queue
 */
void drain_queue(grpc::CompletionQueue& cq) noexcept(false) {
    void* user_data = nullptr;
    bool ok = false;
    while (cq.Next(&user_data, &ok))
        serve_user_data(user_data, ok);
}

/**
 * @brief Fetch and resume queued work with timeout(deadline)
 *
 * @param cq        gRPC completion queue
 * @param deadline  time point to wakeup
 * @return false    Nothing happend
 */
bool drain_queue(
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

/**
 * @brief   Checks if the service shutdown is requested in a low-cost manner
 *
 * @return true     The service should start shutdown
 * @see is_signaled
 */
bool is_shutdowned() noexcept {
    return is_signaled();
}

/**
 * @brief Start gRPC service with the given queue and server instance
 *
 * @param cq    gRPC completion queue
 * @param s     gRPC service launcher
 * @see https://github.com/grpc/grpc/issues/8610
 * @see https://grpc.github.io/grpc/cpp/classgrpc_1_1_server_interface.html
 */
void serve_queue(grpc::CompletionQueue& cq, grpc::Server& s) noexcept(false) {
    auto on_return = gsl::finally([&]() {
        s.Shutdown();  // no more request after shutdown...
        cq.Shutdown(); //  we have to ensure the queue is drained
        return drain_queue(cq);
    });

    using namespace std::chrono;
    using namespace std::chrono_literals;

    // fetch(with timeout) and resume with signal checking
    watch_signals();
    while (is_signaled() == false)
        if (auto shutdowned = drain_queue(cq, system_clock::now() + 1s))
            break;
}
