#pragma once

#include <config.h>

#include <algorithm>
#include <array>
#include <atomic>

namespace bsl {

template <typename Tp, uint64_t Sz>
class ring_buf_t {
 private:
  std::array<Tp, Sz> *ring_buf = nullptr;
  std::atomic<uint64_t> read_head = 0;
  std::atomic<uint64_t> read_commit = 0;
  std::atomic<uint64_t> write_head = 0;
  std::atomic<uint64_t> write_commit = 0;

 public:
  // new assumes no failed allocation
  ring_buf_t() { ring_buf = new std::array<Tp, Sz>(); }
  ring_buf_t(const ring_buf_t &) = delete;
  ring_buf_t(ring_buf_t &&) = delete;
  ring_buf_t &operator=(const ring_buf_t &) = delete;
  ring_buf_t &operator=(ring_buf_t &&) = delete;
  ~ring_buf_t() { delete ring_buf; }

  // non block read
  uint64_t nb_read(Tp *buf, uint64_t size, bool no_partial = false) {
    uint64_t slice_st = read_head;
    uint64_t slice_ed;
    uint64_t slice_len;
    bool get_slice = false;

    // try to get a slice
    while (!get_slice) {
      // sync write_commit load aquire
      auto avail_len =
          (write_commit.load(std::memory_order_acquire) - slice_st + Sz) % Sz;
      if (no_partial && avail_len < size) {
        return 0;
      }
      slice_len = std::min(avail_len, size);
      if (slice_len == 0) {
        return 0;
      }
      slice_ed = (slice_st + slice_len) % Sz;
      get_slice = read_head.compare_exchange_weak(slice_st, slice_ed,
                                                  std::memory_order_relaxed,
                                                  std::memory_order_relaxed);
    }

    // copy data
    if (slice_st > slice_ed) {
      // wrap around
      buf =
          std::copy(ring_buf->begin() + slice_st, ring_buf->begin() + Sz, buf);
      std::copy(ring_buf->begin(), ring_buf->begin() + slice_ed, buf);
    } else {
      std::copy(ring_buf->begin() + slice_st, ring_buf->begin() + slice_ed,
                buf);
    }

    // wait for inorder commit
    while (read_commit.load(std::memory_order_relaxed) != slice_st)
      ;
    // sync read_commit store release
    read_commit.store(slice_ed, std::memory_order_release);
    return slice_len;
  }
  // block read
  Tp read() {
    Tp tmp;
    while (nb_read(&tmp, 1) != 1)
      ;
    return tmp;
  }
  uint64_t read(Tp *buf, uint64_t size, bool no_partial = false) {
    uint64_t ret = 0;
    while ((ret = nb_read(buf, size, no_partial)) == 0)
      ;
    return ret;
  }

  // non block write
  uint64_t nb_write(const Tp *buf, uint64_t size, bool no_partial = false) {
    uint64_t slice_st = write_head;
    uint64_t slice_ed;
    uint64_t slice_len;
    bool get_slice = false;

    // try to get a slice
    while (!get_slice) {
      // sync read_commit load aquire
      // to simplify the logic, ring buffer stores at most Sz - 1 elements
      auto avail_len =
          (read_commit.load(std::memory_order_acquire) - slice_st + Sz - 1) %
          Sz;
      if (no_partial && avail_len < size) {
        return 0;
      }
      slice_len = std::min(avail_len, size);
      if (slice_len == 0) {
        return 0;
      }
      slice_ed = (slice_st + slice_len) % Sz;
      get_slice = write_head.compare_exchange_weak(slice_st, slice_ed,
                                                   std::memory_order_relaxed,
                                                   std::memory_order_relaxed);
    }

    // copy data
    if (slice_st > slice_ed) {
      // wrap around
      auto slice_tmp = slice_len - slice_ed;
      std::copy(buf, buf + slice_tmp, ring_buf->begin() + slice_st);
      std::copy(buf + slice_tmp, buf + slice_len, ring_buf->begin());
    } else {
      std::copy(buf, buf + slice_len, ring_buf->begin() + slice_st);
    }

    // wait for inorder commit
    while (write_commit.load(std::memory_order_relaxed) != slice_st)
      ;
    // sync write_commit store release
    write_commit.store(slice_ed, std::memory_order_release);
    return slice_len;
  }
  // block write
  void write(const Tp &val) {
    while (nb_write(&val, 1) != 1)
      ;
  }
  uint64_t write(const Tp *buf, uint64_t size, bool no_partial = false) {
    uint64_t ret = 0;
    while ((ret = nb_write(buf, size, no_partial)) == 0)
      ;
    return ret;
  }
};

}  // namespace bsl