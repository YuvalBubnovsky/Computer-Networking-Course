.PHONY: clean all

all: myPing mySniffer

myPing: myping.o
	gcc -Wall -g -o myPing myping.o

mySniffer: sniffer.o
	gcc -Wall -g -o mySniffer sniffer.o

sniffer.o: sniffer.c
	gcc -Wall -g -c sniffer.c

myping.o: myping.c
	gcc -Wall -g -c myping.c

clean:
	rm -f *.o *.a *.so myPing mySniffer