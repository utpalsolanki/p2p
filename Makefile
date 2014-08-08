CC=gcc
CFLAGS = -g
LIBS = -lpthread
# uncomment this for SunOS
# LIBS = -lsocket -lnsl

all: peer

peer: peer.o ip-conv.o
	$(CC) -o peer peer.o ip-conv.o $(LIBS)

peer.o: peer.c port.h ip-conv.c ip-conv.h

clean:
	rm -f peer peer.o ip-conv.o
