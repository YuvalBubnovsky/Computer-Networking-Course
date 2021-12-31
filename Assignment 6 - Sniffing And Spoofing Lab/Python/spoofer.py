#!/usr/bin/env python3
from scapy.all import *

ip = IP()
ip.src = '99.99.99.99'
ip.dst = '10.0.2.4'

icmp = ICMP()

packet = ip/icmp

send(packet)