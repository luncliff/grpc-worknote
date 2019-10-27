#include "plugin_core.h"

#if defined(_WIN32)
#include <Windows.h>

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583(v=vs.85).aspx
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID) {
    return TRUE;
}

#else
#include <cstdio>
#include <stdlib.h>

__attribute__((visibility("hidden"))) __attribute__((constructor)) //
void export_mock_functions_env(void) noexcept {
    for (auto label : {"on_attach", "on_destroy", //
                       "on_ready", "on_idle", "on_request_1"})
        setenv(label, label, true);
}

#endif // defined(_WIN32)

plugin_attr_t make_default_attr() noexcept {
    constexpr auto impl_version_code = 0100;

    plugin_attr_t attr{};
    attr.version = impl_version_code;
    attr.platform = PLATFORM_NAME;
    return attr;
}
