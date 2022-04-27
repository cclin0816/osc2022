// #include <bsl/align.h>
// #include <bsl/charconv.h>
// #include <bsl/cstring.h>
#include <fs/cpio.h>
#include <mm.h>
// #include <bsl/string_view.h>

// using namespace bsl::svl;

namespace {

struct cpio_newc_hdr {
  char c_magic[6];
  char c_ino[8];
  char c_mode[8];
  char c_uid[8];
  char c_gid[8];
  char c_nlink[8];
  char c_mtime[8];
  char c_filesize[8];
  char c_devmajor[8];
  char c_devminor[8];
  char c_rdevmajor[8];
  char c_rdevminor[8];
  char c_namesize[8];
  char c_check[8];
  char data[2];
};

cpio_newc_hdr *cpio_base = nullptr;

}  // namespace

namespace fs::cpio {

void set_base(uint64_t base) {
  cpio_base = (cpio_newc_hdr *)mm::phy_to_kn(base);
}

// std::pair<void *, size_t> get_file(const bsl::string_view_t path) {
//   auto cpio_itr = initramfs;
//   while (bsl::sv_t(cpio_itr->c_magic, 6) == "070701"sv) {
//     uint64_t file_len = 0;
//     uint64_t name_len = 0;
//     uint64_t mode = 0;

//     bsl::from_chars(cpio_itr->c_filesize, 8, file_len, 16);
//     bsl::from_chars(cpio_itr->c_namesize, 8, name_len, 16);
//     bsl::from_chars(cpio_itr->c_mode, 8, mode, 16);
//     name_len -= 1;

//     if (mode == 0 && strcmp(cpio_itr->data, "TRAILER!!!") == 0) {
//       break;
//     }
//     if (bsl::sv_t(cpio_itr->data, name_len) == path) {
//       return {(void *)(&(cpio_itr->data[2 + bsl::p2align_up(name_len - 1, 4)])),
//               file_len};
//     }
//     cpio_itr =
//         (cpio_newc_hdr *)&(cpio_itr->data[2 + bsl::p2align_up(name_len - 1, 4) +
//                                           bsl::p2align_up(file_len, 4)]);
//   }
//   return {PTR_FAIL, 0};
// }

}  // namespace fs::cpio
