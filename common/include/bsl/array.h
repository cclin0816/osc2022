#pragma once

#include <array>

namespace bsl {

template<typename T, std::size_t N>
using array_t = std::array<T, N>;

}  // namespace bsl