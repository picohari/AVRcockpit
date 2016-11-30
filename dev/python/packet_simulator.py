import socket
import sys

# addressing information of target
IPADDR = '192.168.1.131'
PORTNUM = 6788

# protocol definitions
STX = 0x02
NR = 0x0000
RS = 0x1E
ETX = 0x03


# insert dummy data here:

WHAT = 0x0018

TYPE = 0x01

VALUE = 0x01


# build packet datagram
PACKETDATA = bytearray(b"")

PACKETDATA.append( STX )

PACKETDATA.append( NR & 0xFF )
PACKETDATA.append((NR >> 8) & 0xFF  )

PACKETDATA.append( RS )

PACKETDATA.append( WHAT & 0xFF )
PACKETDATA.append((WHAT >> 8) & 0xFF )

PACKETDATA.append( TYPE )


PACKETDATA.append( VALUE & 0xFF )
#if VALUE > 0xFF :
#PACKETDATA.append((VALUE >> 8) & 0xFF )
#if VALUE > 0xFFFF :
#PACKETDATA.append((VALUE >> 16) & 0xFF )
#if VALUE > 0xFFFFFF :
#PACKETDATA.append((VALUE >> 32) & 0xFF )


PACKETDATA.append( ETX )

# Illumination [on]
#PACKETDATA = '0200001e1800010003'.decode('hex')

# Blinker [freq][mode]
#PACKETDATA = '0200001e2000400000000003'.decode('hex')

#print(PACKETDATA)
print(''.join(format(x, '02x') for x in PACKETDATA))
print("%d Bytes" % len(PACKETDATA))

#print(len(VALUE)) 

# initialize a socket, think of it as a cable
# SOCK_DGRAM specifies that this is UDP
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)

try:
    # connect the socket, think of it as connecting the cable to the address location
    s.connect((IPADDR, PORTNUM))

    # send the command
    s.send(PACKETDATA)
except:
    pass

# close the socket
s.close()

