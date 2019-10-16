#pragma once

#include <grpcpp/grpcpp.h>
#include <plugin_core.h>

#if __has_include(<coroutine>)
#include <coroutine>
using namespace std;

#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
using namespace std;
using namespace std::experimental;

#endif

struct grpc_await_context_t {
    function<void(coroutine_handle<void>)> callback;

    void set_false() noexcept {
        callback = nullptr;
    }

    bool await_ready() noexcept {
        return false;
    }
    void await_suspend(coroutine_handle<void> coro) noexcept(false) {
        return callback(coro);
    }
    bool await_resume() noexcept {
        return static_cast<bool>(callback);
    }
};

struct promise_grpc_service_routine_t : public grpc_await_context_t {
    auto initial_suspend() noexcept {
        return suspend_never{};
    }
    auto final_suspend() noexcept {
        return suspend_never{};
    }
    void unhandled_exception() noexcept(false) {
        throw;
    }
    void return_value(grpc::Status status) noexcept {
    }

    auto get_return_object() noexcept {
        return this;
    }
    auto
    await_transform(function<void(coroutine_handle<void>)>&& callback) noexcept
        -> grpc_await_context_t& {
        this->callback = std::move(callback);
        return *this;
    }
};

struct grpc_service_routine_t : coroutine_handle<void> {
    using promise_type = promise_grpc_service_routine_t;

    grpc_service_routine_t(promise_type* p)
        : coroutine_handle<void>{
              coroutine_handle<promise_type>::from_promise(*p)} {
    }
};
