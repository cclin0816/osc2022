#include <mmio.h>
#include <watchdog.h>

namespace watchdog {

constexpr auto PM_PW = 0x5a000000U;
constexpr auto PM_RSTC = 0x10001cU;
constexpr auto PM_WDOG = 0x100024U;

void start(uint32_t tick) {
  mmio::w32(PM_WDOG, PM_PW | tick);
  mmio::w32(PM_RSTC, PM_PW | 0x20);
}

void refresh(uint32_t tick) { mmio::w32(PM_WDOG, PM_PW | tick); }

void stop() { mmio::w32(PM_RSTC, PM_PW | 0x0); }

}  // namespace watchdog