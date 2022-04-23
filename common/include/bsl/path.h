#pragma once

#include <bsl/pair.h>
#include <bsl/string_view.h>

namespace bsl {

constexpr bsl::pair_t<bsl::sv_t, bsl::sv_t> lsplit_path(bsl::sv_t path) {
  bsl::pair_t<bsl::sv_t, bsl::sv_t> ret;
  auto pos = path.find_first_of('/');
  if (pos == path.npos) {
    ret.first = path;
  } else {
    ret.first = path.substr(0, pos);
    ret.second = path.substr(pos + 1);
  }
  return ret;
}

constexpr bsl::pair_t<bsl::sv_t, bsl::sv_t> rsplit_path(bsl::sv_t path) {
  bsl::pair_t<bsl::sv_t, bsl::sv_t> ret;
  auto pos = path.find_last_of('/');
  if (pos == path.npos) {
    ret.second = path;
  } else {
    ret.first = path.substr(0, pos);
    ret.second = path.substr(pos + 1);
  }
  return ret;
}

}  // namespace bsl