#pragma once

#include <config.h>

#include <new>
#include <utility>

namespace bsl {

template <typename T, size_t _capacity>
class static_vec {
 private:
  uint64_t _size = 0;
  T _data[_capacity] = {};

 public:

  // push back forward argument to data
  template <typename... Args>
  void push_back(Args &&...args) {
    if (_size < _capacity) {
      new (_data + _size) T(std::forward<Args>(args)...);
      _size++;
    }
  }
  void pop_back() {
    _size--;
    _data[_size].~T();
  }
  [[nodiscard]] uint64_t size() const { return _size; }
  [[nodiscard]] uint64_t capacity() const { return _capacity; }
  [[nodiscard]] bool empty() const { return _size == 0; }

  T &operator[](uint64_t idx) { return _data[idx]; }
  T &back() { return _data[_size - 1]; }
  T &front() { return _data[0]; }
  T *data() { return _data; }
  T *begin() { return _data; }
  T *end() { return _data + _size; }

  const T &operator[](uint64_t idx) const { return _data[idx]; }
  [[nodiscard]] const T &back() const { return _data[_size - 1]; }
  [[nodiscard]] const T &front() const { return _data[0]; }
  [[nodiscard]] const T *data() const { return _data; }
  [[nodiscard]] const T *begin() const { return _data; }
  [[nodiscard]] const T *end() const { return _data + _size; }
};

}  // namespace bsl