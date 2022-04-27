#pragma once

#include <bsl/cstring.h>
#include <config.h>

#include <string_view>

namespace bsl {

using string_view_t = std::string_view;
using sv_t = string_view_t;

inline sv_t sv_slow(const char *str) { return {str, strlen_slow(str)}; }

namespace svl = std::literals::string_view_literals;

}  // namespace bsl