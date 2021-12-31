.PHONY: clean all

all: sniffer spoofer spnoof

spnoof:
	gcc -o spnoof SniffSnoop.c -lpcap

sniffer:
	gcc -o sniffer sniffer.c -lpcap

spoofer:
	gcc -o spoofer spoofer.c

clean:
	rm -f *.o *.a *.so sniffer spoofer spnoof