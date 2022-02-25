#pragma once

#include <config.h>

namespace watchdog {

void start(uint32_t tick);

void refresh(uint32_t tick);

void stop();

}  // namespace watchdog