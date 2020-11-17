all: atdate

CFLAGS=-include /usr/include/errno.h

atdate: atdate.c clientUDP.c clientTCP.c serverTCP.c clientUDP.h clientTCP.h serverTCP.h

clean:
	rm -f *.o atdate

