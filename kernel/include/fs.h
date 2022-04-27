#pragma once

#include <bsl/pair.h>
#include <bsl/string_view.h>
#include <conf.h>
#include <config.h>

namespace fs {

void init();

namespace cpio {

std::pair<void *, size_t> get_file(const bsl::sv_t path);

}  // namespace cpio

}  // namespace fs