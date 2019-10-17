
#include "loader.h"

#if __has_include(<Windows.h>)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

loader_t::~loader_t() noexcept {
    if (handle)
        ::FreeLibrary(static_cast<HMODULE>(handle));
}

loader_t loader_t::load(const char* file) noexcept {
    return loader_t{::LoadLibraryA(file)};
}

auto loader_t::lookup(const char* symbol) const noexcept -> void* {
    return ::GetProcAddress(static_cast<HMODULE>(handle), symbol);
}

auto loader_t ::error() noexcept -> const char* {
    return strerror(static_cast<errno_t>(GetLastError()));
}

#elif __has_include(<dlfcn.h>)
#include <dlfcn.h>

loader_t::~loader_t() noexcept {
    if (handle)
        ::dlclose(handle));
}
loader_t loader_t::load(const char* file) noexcept {
    return loader_t{::dlopen(file, RTLD_NOW)};
}

auto loader_t::lookup(const char* symbol) const noexcept -> void* {
    return ::dlsym(handle, symbol);
}

auto loader_t ::error() noexcept -> const char* {
    return ::dlerror();
}

#endif
