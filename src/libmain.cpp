
#include <filesystem>
#include <plugin_core.h>

namespace fs = std::filesystem;

constexpr auto impl_version_code = 0100;

plugin_attr_t make_default_attr() noexcept {
    plugin_attr_t attr{};
    attr.version = impl_version_code;
    attr.platform = PLATFORM_NAME;
    attr.workspace = fs::current_path().generic_wstring();
    return attr;
}

#if defined(_WIN32)
#include <Windows.h>

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583(v=vs.85).aspx
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID) {
    return TRUE;
}

#endif // defined(_WIN32)
