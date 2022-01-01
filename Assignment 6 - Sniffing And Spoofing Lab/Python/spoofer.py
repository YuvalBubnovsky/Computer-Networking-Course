#!/usr/bin/env python3
from scapy.all import *

#Simple Scapy script to spoof ICMP replies
ip = IP()
ip.src = '99.99.99.99'
ip.dst = '10.0.2.4'

icmp = ICMP()

packet = ip/icmp

send(packet)