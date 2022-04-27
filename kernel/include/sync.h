// #pragma once

// #include <conf.h>
// #include <config.h>
// #include <excep.h>

// namespace sync {

// struct no_lck {};

// class irq_lck {
//   bool prev_state = false;
//   bool locked = false;

//  public:
//   irq_lck() { lock(); }
//   irq_lck(no_lck) {}
//   irq_lck(const irq_lck &) = delete;
//   irq_lck(irq_lck &&) = delete;
//   irq_lck &operator=(const irq_lck &) = delete;
//   irq_lck &operator=(irq_lck &&) = delete;
//   ~irq_lck() { unlock(); }
//   void lock() {
//     if (!locked) {
//       prev_state = excep::is_enabled();
//       if (prev_state) {
//         excep::disable();
//       }
//       locked = true;
//     }
//   }
//   void unlock() {
//     if (locked) {
//       if (prev_state) {
//         excep::enable();
//       }
//       locked = false;
//     }
//   }
// };

// class cpu_lck : irq_lck {
//  public:
//   cpu_lck() = default;
//   ~cpu_lck() = default;
// };

// }  // namespace sync