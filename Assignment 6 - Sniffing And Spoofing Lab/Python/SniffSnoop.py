#!/usr/bin/env python3
from scapy.all import *

# Simple Scapy script to sniff ICMP packets on the web and spoof replies
def spoof(pkt):
    # code 8 is echo, we don't care for anything else
    if pkt[ICMP].type != 8:
        return

    # cooking the spoofed packet using all the information from
    # the original one
    ip = IP(src=pkt[IP].dst, dst=pkt[IP].src, ihl=pkt[IP].ihl)
    icmp = ICMP(type=0, id=pkt[ICMP].id, seq=pkt[ICMP].seq)
    payload = pkt[Raw].load
    spoofed_pkt = ip/icmp/payload
    send(spoofed_pkt, verbose=0)

    print("Sent spoofed ICMP packet intended for " + str(pkt[IP].dst) + " to " + str(pkt[IP].src))

while(True):
    pkt = sniff(filter='icmp', prn=spoof)