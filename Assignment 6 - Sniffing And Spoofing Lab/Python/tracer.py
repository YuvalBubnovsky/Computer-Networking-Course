#!/usr/bin/env python3
from scapy.all import *
import sys

# Setting max hops to 200
MAX_TTL = 200

dest_name = sys.argv[1]
dest_IP = socket.gethostbyname(sys.argv[1])
print("tracing to "+ dest_name + " [" + dest_IP + "], Max TTL : 200")

# Creating the packet
ip = IP()
ip.dst = dest_IP
ip.ttl = 1
icmp = ICMP()

# Incrementing TTL until we reach target or timeout
while ip.ttl <= MAX_TTL:
    # cooking the packet and sending it
    r = sr1(ip/icmp, verbose = 0, timeout = 2)

    # If no reply is recieved, we skip to the next TTL
    if(r == None):
        print(str(ip.ttl) + "\t* * * * *")
        ip.ttl+=1
        continue
    
    # Pretty print the result
    print(str(ip.ttl) + "\t" + r.src)
    
    if(r.src == dest_IP):
        break

    # If all else fails, add to TTL
    ip.ttl+=1

