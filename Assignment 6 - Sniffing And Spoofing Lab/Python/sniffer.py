#!/usr/bin/env python3
from scapy.all import *

# Simple Scapy scrpit to sniff packets
# filter is currently defined to listen to a specific subnet, can be changed according to BPF
def print_pkt(pkt):
    pkt.show()

pkt = sniff(filter='net 128.230.0.0/16', prn=print_pkt)


