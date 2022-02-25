#pragma once

#include <utility>

namespace bsl {

// template <typename T, typename U>
// struct pair_t {
//   T first{};
//   U second{};
// };

template <typename T, typename U>
using pair_t = std::pair<T, U>;

}  // namespace bsl