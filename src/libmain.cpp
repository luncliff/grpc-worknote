#include <service_core.h>

#if defined(_WIN32)
#include <Windows.h>

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583(v=vs.85).aspx
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID) {
  return TRUE;
}

#endif // defined(_WIN32)
