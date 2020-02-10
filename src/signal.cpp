/**
 * @file signal.cpp
 * @author luncliff (luncliff@gmail.com)
 * @brief Signal handling for the service module
 * @details The signal is used for graceful shutdown of the service
 * implementation
 */
// #include <cstdio>
#include <csignal>

volatile sig_atomic_t sig_status = 0;

bool is_signaled() noexcept {
    return sig_status != 0;
}

void on_signal_change_sig_status(sig_atomic_t signum) {
    sig_status = signum;
    // fprintf(stderr, "signal: %d \n", signum);
}

void watch_signals() noexcept(false) {
    // hijack some signals. it will break the completion queue
    signal(SIGTERM, on_signal_change_sig_status);
}
