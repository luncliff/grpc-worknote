
#include "plugin_impl.h"

using namespace std;

auto str2span(const string& s) noexcept -> gsl::span<const std::byte> {
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
    return str2span(s);
}

void set_blob(plugin_response_t& res, gsl::span<std::byte> view) noexcept {
    return res.set_blob(view.data(), view.size_bytes());
}

auto get_blob(const plugin_response_t& res) noexcept
    -> gsl::span<const std::byte> {
    const auto& s = res.blob();
    return str2span(s);
}

// todo: replace to arena allocator
// google::protobuf::Arena arena{};

auto make_request() noexcept(false) -> unique_ptr<plugin_request_t> {
    auto req = make_unique<plugin_request_t>();
    return req;
}

auto make_response() noexcept(false) -> unique_ptr<plugin_response_t> {
    auto res = make_unique<plugin_response_t>();
    return res;
}
