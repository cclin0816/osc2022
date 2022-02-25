import pwn
from functools import reduce

def send_kernel(serial, kernel_file):
  kn_file = open(kernel_file, 'rb').read()

  print("Wait handshake")
  while True:
    serial.send(b'\x54\x87')
    ret = serial.recvuntil(b'\x54\x87', timeout=0.1)
    if ret != b'':
      serial.send(b'\xff\xff')
      break

  while True:

    print("Send kernel")
    serial.send(pwn.p32(len(kn_file)))
    serial.send(kn_file)
    checksum = reduce(lambda x, y: x ^ y, kn_file)
    checksum_res = serial.recv(1)
    checksum = checksum.to_bytes(1, 'little')
    if(checksum == checksum_res):
      serial.send(b'\x00')
      print("Checksum OK")
      break
    
    print("Checksum NG")
    print(checksum, checksum_res)
    serial.send(b'\x01')

  # ptr = pwn.u64(serial.recv(8))
  # print(hex(ptr))
  # size = pwn.u32(serial.recv(4))
  # print(hex(size))
  # data = b''
  # while len(data) < size:
  #     data += serial.recv(256)
  #     serial.send(b'\x00')
  #     # print(len(data))

  # print(len(data))
  # open('fdt.dtb', 'wb').write(data)