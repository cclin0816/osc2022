#include <bsl/cassert.h>
#include <start.h>

[[noreturn]] void _abort() noexcept { halt(); }

void abort_msg([[maybe_unused]] const char* msg,
               [[maybe_unused]] const char* file,
               [[maybe_unused]] unsigned line,
               [[maybe_unused]] const char* func) noexcept {}