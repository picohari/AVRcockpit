import socket

# addressing information of target
IPADDR = '192.168.1.131'
PORTNUM = 6788

# enter the data content of the UDP packet as hex
PACKETDATA = '30005d'.decode('hex')

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
