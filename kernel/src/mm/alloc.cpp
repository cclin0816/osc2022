#include <bsl/align.h>
#include <bsl/cassert.h>
#include <bsl/cstring.h>
#include <kn_conf.h>
#include <mm.h>

namespace {

bool buddy_init = false;

}  // namespace

namespace mm {

void *kmalloc(uint64_t size) {
  if (size == 0) [[unlikely]] {
    return PTR_FAIL;
  }
  if (size >= PAGE_SZ) {
    size = bsl::p2align_up(size, PAGE_SZ);
    if (buddy_init) [[likely]] {
      alloc_page(size);
    } else {
      return early_alloc_page(size);
    }
  }
  todo();
  return PTR_FAIL;
}

void *kzalloc(uint64_t size) {
  auto *ptr = kmalloc(size);
  if (ptr != PTR_FAIL) {
    bsl::memset(ptr, 0, size);
  }
  return ptr;
}

void kfree([[maybe_unused]] void *ptr) { todo(); }

void *alloc_page(uint64_t size) {
  todo();
  return PTR_FAIL;
}

}  // namespace mm