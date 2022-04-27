#pragma once

#include <bsl/pair.h>
#include <bsl/string_view.h>

namespace fs {

namespace cpio {

void init(void *);
std::pair<void *, size_t> get_file(const bsl::string_view_t);

} // namespace cpio

}  // namespace fs