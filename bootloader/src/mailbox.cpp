#include <mailbox.h>
#include <mmio.h>

namespace mailbox {

constexpr auto BASE = 0xb880U;

constexpr auto READ = BASE;
// constexpr auto READ_PEEK = BASE + 0x10U;
// constexpr auto READ_SENDER = BASE + 0x14U;
constexpr auto READ_STATUS = BASE + 0x18U;
// constexpr auto READ_CONFIG = BASE + 0x1CU;

constexpr auto WRITE = BASE + 0x20U;
// constexpr auto WRITE_PEEK = BASE + 0x30U;
// constexpr auto WRITE_SENDER = BASE + 0x34U;
constexpr auto WRITE_STATUS = BASE + 0x38U;
// constexpr auto WRITE_CONFIG = BASE + 0x3CU;

// constexpr auto CH_PWM = 0U;
// constexpr auto CH_FRAMEBUF = 1U;
// constexpr auto CH_VUART = 2U;
// constexpr auto CH_VCHIQ = 3U;
// constexpr auto CH_LED = 4U;
// constexpr auto CH_BTN = 5U;
// constexpr auto CH_TOUCHSCREEN = 6U;
constexpr auto CH_PROP_ARM_VC = 8U;
// constexpr auto CH_PROP_VC_ARM = 9U;

constexpr auto EMPTY = 0x40000000U;
constexpr auto FULL = 0x80000000U;

constexpr auto GET_FIRMWARE_REV = 0x00000001U;
constexpr auto GET_BOARD_MODEL = 0x00010001U;
constexpr auto GET_BOARD_REVISION = 0x00010002U;
constexpr auto GET_BOARD_MAC_ADDRESS = 0x00010003U;
constexpr auto GET_BOARD_SERIAL = 0x00010004U;
constexpr auto GET_ARM_MEMORY = 0x00010005U;
constexpr auto GET_VC_MEMORY = 0x00010006U;
// constexpr auto GET_CLOCKS = 0x00010007U;

constexpr auto REQUEST_CODE = 0x0U;
constexpr auto REQUEST_SUCCEED = 0x80000000U;
// constexpr auto REQUEST_FAILED = 0x80000001U;
constexpr auto TAG_REQUEST_CODE = 0x0U;
constexpr auto END_TAG = 0x0U;

/**
 * @brief generate mailbox request
 * @param addr address of message, must be 16-byte aligned low memory(32bit)
 * @param channel mailbox channel
 * @return uint32_t request
 */
uint32_t gen_req(void *addr, uint8_t channel) {
  return ((((uint64_t)addr) & 0xfffffff0) | (channel & 0xf));
}

/**
 * @brief wait for mailbox 1 fifo writable
 */
void wait_writable() {
  while ((mmio::r32(WRITE_STATUS) & FULL) != 0)
    ;
}

/**
 * @brief wait for mailbox 0 fifo readable
 */
void wait_readable() {
  while ((mmio::r32(READ_STATUS) & EMPTY) != 0)
    ;
}

/**
 * @brief blocked write request
 * @param req request
 */
void send(uint32_t req) {
  wait_writable();
  mmio::w32(WRITE, req);
}

/**
 * @brief blocked until request fulfilled
 * @param req request
 */
void recv(uint32_t req) {
  while (true) {
    wait_readable();
    uint32_t res = mmio::r32(READ);
    if (res == req) break;
  }
}

/**
 * @brief one shot mailbox request
 * @param msg_addr address of message, must be 16-byte aligned low memory(32bit)
 * @param channel mailbox channel
 */
void req(void *msg_addr, uint8_t channel) {
  uint32_t req = gen_req(msg_addr, channel);
  send(req);
  recv(req);
}

template <typename T>
struct __attribute__((aligned(16))) tag_msg {
 public:
  uint32_t msg_size = sizeof(T) + 6 * 4;
  uint32_t req_code = REQUEST_CODE;
  uint32_t tag_id = 0;
  uint32_t buf_len = sizeof(T);
  uint32_t tag_code = TAG_REQUEST_CODE;
  T buf{};
  uint32_t end_tag = END_TAG;
  tag_msg(uint32_t _tag_id) : tag_id(_tag_id) {}
  tag_msg(uint32_t _tag_id, T _buf) : tag_id(_tag_id), buf(_buf) {}
};

/**
 * @brief Get the board revision number
 * @return uint32_t revision number, 0x0 if failed
 */
uint32_t get_board_rev() {
  tag_msg<uint32_t> m(GET_BOARD_REVISION);
  req(&m, CH_PROP_ARM_VC);
  if (m.req_code != REQUEST_SUCCEED) return 0;
  return m.buf;
}

/**
 * @brief Get the arm memory
 * @return uint32_t pair: base size, 0x0 0x0 if failed
 */
bsl::pair_t<uint32_t, uint32_t> get_arm_mem() {
  tag_msg<bsl::pair_t<uint32_t, uint32_t>> m(GET_ARM_MEMORY);
  req(&m, CH_PROP_ARM_VC);
  if (m.req_code != REQUEST_SUCCEED) return bsl::pair_t<uint32_t, uint32_t>{};
  return m.buf;
}

uint32_t get_firm_rev() {
  tag_msg<uint32_t> m(GET_FIRMWARE_REV);
  req(&m, CH_PROP_ARM_VC);
  if (m.req_code != REQUEST_SUCCEED) return 0;
  return m.buf;
}

uint32_t get_board_model() {
  tag_msg<uint32_t> m(GET_BOARD_MODEL);
  req(&m, CH_PROP_ARM_VC);
  if (m.req_code != REQUEST_SUCCEED) return 0;
  return m.buf;
}

uint64_t get_board_mac() {
  tag_msg<uint64_t> m(GET_BOARD_MAC_ADDRESS);
  req(&m, CH_PROP_ARM_VC);
  if (m.req_code != REQUEST_SUCCEED) return 0;
  return m.buf;
}

uint64_t get_board_serial() {
  tag_msg<uint64_t> m(GET_BOARD_SERIAL);
  req(&m, CH_PROP_ARM_VC);
  if (m.req_code != REQUEST_SUCCEED) return 0;
  return m.buf;
}

bsl::pair_t<uint32_t, uint32_t> get_vc_mem() {
  tag_msg<bsl::pair_t<uint32_t, uint32_t>> m(GET_VC_MEMORY);
  req(&m, CH_PROP_ARM_VC);
  if (m.req_code != REQUEST_SUCCEED) return bsl::pair_t<uint32_t, uint32_t>{};
  return m.buf;
}

}  // namespace mailbox