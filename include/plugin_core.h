//
//  Author
//      github.com/luncliff
//
#pragma once
#ifndef PLUGIN_CORE_H
#define PLUGIN_CORE_H

#include <gsl/gsl> // see: github.com/microsoft/gsl
#include <memory>

struct plugin_attr_t final {
    uint64_t version;
    gsl::cwzstring<> platform;
};

plugin_attr_t make_default_attr() noexcept;

struct plugin_context_t;
using plugin_create_t = auto (*)(const plugin_attr_t& attr) noexcept
                        -> plugin_context_t*;
using plugin_destroy_t = void (*)(plugin_context_t*) noexcept;

using plugin_status_t = void (*)(gsl::not_null<plugin_context_t*> p) noexcept;

struct plugin_request_t;
struct plugin_response_t;

void set_blob(plugin_request_t& req, gsl::span<std::byte> view) noexcept;
auto get_blob(const plugin_request_t& req) noexcept
    -> gsl::span<const std::byte>;

void set_blob(plugin_response_t& res, gsl::span<std::byte> view) noexcept;
auto get_blob(const plugin_response_t& res) noexcept
    -> gsl::span<const std::byte>;

using plugin_error_t = uint32_t;
// auto plugin_error_string(plugin_error_t err) -> gsl::czsring<>;

using plugin_serve_t = auto (*)(gsl::not_null<plugin_context_t*> p, //
                                const plugin_request_t& req,
                                plugin_response_t& res, bool& ready) noexcept
                       -> plugin_error_t;

struct plugin_t final {
    gsl::not_null<plugin_create_t> on_create;
    gsl::not_null<plugin_destroy_t> on_destroy;
    gsl::not_null<plugin_status_t> on_ready;
    gsl::not_null<plugin_status_t> on_idle;
    gsl::not_null<plugin_serve_t> on_serve_1;
    gsl::not_null<plugin_serve_t> on_serve_2;
    gsl::not_null<plugin_serve_t> on_serve_3;
    gsl::not_null<plugin_serve_t> on_serve_4;
};

#endif // PLUGIN_CORE_H
