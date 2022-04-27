#pragma once

#include <config.h>

namespace bsl {

struct cdll_t {
  cdll_t *prev = nullptr;
  cdll_t *next = nullptr;
  void init() {
    prev = this;
    next = this;
  }
  void push_front(cdll_t *node) {
    node->prev = prev;
    node->next = this;
    prev->next = node;
    prev = node;
  }
  void push_back(cdll_t *node) {
    node->prev = this;
    node->next = next;
    next->prev = node;
    next = node;
  }
  bool empty() { return prev == this; }
  cdll_t *pop_back() {
    cdll_t *node = next;
    next = node->next;
    next->prev = this;
    node->prev = node;
    node->next = node;
    return node;
  }
  cdll_t *pop_front() {
    cdll_t *node = prev;
    prev = node->prev;
    prev->next = this;
    node->prev = node;
    node->next = node;
    return node;
  }
  cdll_t *remove() {
    prev->next = next;
    next->prev = prev;
    prev = this;
    next = this;
    return this;
  }
  uint64_t size() {
    uint64_t size = 0;
    for (cdll_t *node = next; node != this; node = node->next) {
      size++;
    }
    return size;
  }
};

}  // namespace bsl