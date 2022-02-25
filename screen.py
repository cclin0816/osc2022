from code import interact
import pwn
import argparse
from send_kernel import *

parser = argparse.ArgumentParser()
parser.add_argument('-t', '--tty', help='target tty', default='/dev/ttyUSB0')
parser.add_argument('-b', '--baudrate',
                    help='serial baudrate', type=int, default=921600)
parser.add_argument('-k', '--kernel', help='kernel file',
                    default='kernel/build/kernel.img')
# -i to interact option
parser.add_argument('-i', '--interact', help='interact with the kernel',
                    action='store_true')
args = parser.parse_args()

tty = args.tty
baudrate = args.baudrate
kernel_file = args.kernel
interact = args.interact

serial = pwn.serialtube(tty, baudrate=baudrate,  convert_newlines=False)

if(not interact):
  send_kernel(serial, kernel_file)

serial.interactive()
