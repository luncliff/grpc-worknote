
// Unlike python2.7 subprocess module, this library currently only supports MAC OS and Linux. It has no support for Windows in its current state.
#include "subprocess.hpp"
#include <service_core.h>

using namespace std;
using namespace subprocess;

uint32_t execute_failed(const exception &) noexcept { return UINT16_MAX; }

uint32_t execute_sync(initializer_list<const char *> argv) noexcept {
  try {
    auto retcode = call(argv);
    return gsl::narrow_cast<uint32_t>(retcode);

  } catch (const exception &ex) {
    return execute_failed(ex);
  }
}

uint32_t execute_sync(initializer_list<const char *> argv,
                      uint64_t ofd) noexcept {
  try {
    auto retcode = call(argv, output{gsl::narrow<int>(ofd)});
    return gsl::narrow_cast<uint32_t>(retcode);

  } catch (const exception &ex) {
    return execute_failed(ex);
  }
}

uint32_t execute_sync(initializer_list<const char *> argv, uint64_t ifd,
                      uint64_t ofd) noexcept {
  try {
    auto retcode = call(argv, input{gsl::narrow<int>(ifd)}, //
                        output{gsl::narrow<int>(ofd)});
    return gsl::narrow_cast<uint32_t>(retcode);

  } catch (const exception &ex) {
    return execute_failed(ex);
  }
}
