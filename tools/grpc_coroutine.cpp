
#include <plugin_core.h>
#include <service.grpc.pb.h>

#include "grpc_coroutine.h"

using namespace std;

void consume_queue(grpc::Server& launcher,
                   grpc::CompletionQueue& cq) noexcept(false);

auto handle_invoke1(grpc_impl::ServerCompletionQueue& queue,
                    plugins::Executor1::AsyncService& service) noexcept(false)
    -> service_coroutine_t;

auto handle_invoke2(grpc_impl::ServerCompletionQueue& queue,
                    plugins::Executor1::AsyncService& service) noexcept(false)
    -> service_coroutine_t;

auto handle_invoke3(grpc_impl::ServerCompletionQueue& queue,
                    plugins::Executor1::AsyncService& service) noexcept(false)
    -> service_coroutine_t;

auto handle_invoke4(grpc_impl::ServerCompletionQueue& queue,
                    plugins::Executor1::AsyncService& service) noexcept(false)
    -> service_coroutine_t;

uint32_t serve(grpc::Server& launcher, //
               grpc_impl::ServerCompletionQueue& queue,
               plugins::Executor1::AsyncService& service) noexcept try {
    handle_invoke1(queue, service);
    handle_invoke2(queue, service);
    handle_invoke3(queue, service);
    handle_invoke4(queue, service);

    consume_queue(launcher, queue);
    return 0;
} catch (const std::exception& ex) {
    return -1;
}

void consume_queue(grpc::Server& launcher,
                   grpc::CompletionQueue& cq) noexcept(false) {
    // we have to drain the queue.
    // see https://github.com/grpc/grpc/issues/8610
    auto on_return_1 = gsl::finally([q = &cq]() {
        q->Shutdown();
        void* user_data{};
        bool ok{};
        while (q->Next(&user_data, &ok))
            ;
    });
    // make sure of the shutdown
    auto on_return_2 = gsl::finally([s = &launcher]() { s->Shutdown(); });

    void* user_data = nullptr;
    bool ok = false;

    // dispatch and continue service coroutines
    while (cq.Next(&user_data, &ok) == true) {
        using promise_t = promise_service_coroutine_t;
        auto task = coroutine_handle<promise_t>::from_address(user_data);
        if (ok == false)
            task.promise().set_false();

        if (task && task.done() == false)
            task.resume();
    }
}
