
#include "grpc_plugin.h"

using namespace std;

auto str2byte(const string& s) noexcept -> gsl::span<const std::byte> {
    auto* ptr = reinterpret_cast<const std::byte*>(s.data());
    auto len = static_cast<ptrdiff_t>(s.length());
    return {ptr, len};
}

void set_blob(plugin_request_t& req, gsl::span<std::byte> view) noexcept {
    return req.set_blob(view.data(), view.size_bytes());
}

auto get_blob(const plugin_request_t& req) noexcept
    -> gsl::span<const std::byte> {
    const auto& s = req.blob();
    return str2byte(s);
}

void set_blob(plugin_response_t& res, gsl::span<std::byte> view) noexcept {
    return res.set_blob(view.data(), view.size_bytes());
}

auto get_blob(const plugin_response_t& res) noexcept
    -> gsl::span<const std::byte> {
    const auto& s = res.blob();
    return str2byte(s);
}

