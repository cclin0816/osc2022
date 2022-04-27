// #include <mm/types.h>

// #include <algorithm>

// namespace mm {

// bool collide_any(const mem_region_t *begin, const mem_region_t *end) {
//   if (std::distance(begin, end) < 2) {
//     return false;
//   }
//   auto *first = begin;
//   auto *next = begin + 1;
//   while (next != end) {
//     if (first->collide(*next)) {
//       return true;
//     }
//     first = next;
//     ++next;
//   }
//   return false;
// }

// }  // namespace mm