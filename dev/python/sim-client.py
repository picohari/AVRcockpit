#!/usr/bin/python

import signal
import time
import sys
import socket


import smbus

bus = smbus.SMBus(0)    # 0 = /dev/i2c-0 (port I2C0), 1 = /dev/i2c-1 (port I2C1)

GEAR_IND_ADDRESS = 0x10	#7 bit address (will be left shifted to add the read write bit)
GEAR_IND_PRINT   = 0x10
GEAR_IND_PWMA	 = 0x20



delay = 0.14	# ok: 0.05

# Set up a UDP server
udp_socket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

# Save own ip adress, see: http://stackoverflow.com/a/1267524
ip_host = ([(s.connect(('8.8.8.8', 80)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1])

# Listen on port 10000
# (to all IP addresses on this system)
listen_addr = ("",10000)
udp_socket.bind(listen_addr)


# Data structure:
# ----------------------------
# POS : LENGTH : COMMAND          : VALUE : RANGE
#   0 : 2 Byte : GEAR_IND_PRINT   : hex   : 0 ... where 0 = R, 1 = N, 2 = first, 3 = second .... 
#   2 : 2 Byte : 



class color:
   PURPLE = '\033[95m'
   CYAN = '\033[96m'
   DARKCYAN = '\033[36m'
   BLUE = '\033[94m'
   GREEN = '\033[92m'
   YELLOW = '\033[93m'
   RED = '\033[91m'
   BOLD = '\033[1m'
   UNDERLINE = '\033[4m'
   END = '\033[0m'


def init_program():
    print("\n Hardware UDP Simulator Server - Version 0.2.1")
    print("-------------------------------------------------")
    print color.YELLOW + color.BOLD + ' This machine has IP: ' + color.RED + ip_host + color.END
    print("-------------------------------------------------")




def run_program():

    init_program()


    while True:
        data, addr = udp_socket.recvfrom(1024)

	print data[0]
        print data.strip(), addr


	bus.write_byte_data(GEAR_IND_ADDRESS, GEAR_IND_PRINT, 0x30)


def exit_gracefully(signum, frame):
    # restore the original signal handler as otherwise evil things will happen
    # in raw_input when CTRL+C is pressed, and our signal handler is not re-entrant
    signal.signal(signal.SIGINT, original_sigint)

    try:
        if raw_input("\nReally quit? (y/n) ").lower().startswith('y'):
	    udp_socket.close()
            sys.exit(1)
	else:
	    run_program()

    except KeyboardInterrupt:
	print(" - Bye!\n")
        udp_socket.close()
        sys.exit(1)

    # restore the exit gracefully handler here    
    signal.signal(signal.SIGINT, exit_gracefully)


if __name__ == '__main__':
    # store the original SIGINT handler
    original_sigint = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, exit_gracefully)
    run_program()


