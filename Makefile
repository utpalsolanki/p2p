CC=gcc
CFLAGS = -g
LIBS = -lpthread
# uncomment this for SunOS
# LIBS = -lsocket -lnsl

all: peer

peer: peer.o ip-conv.o peer_parser.o
	$(CC) -o peer peer.o ip-conv.o peer_parser.o $(LIBS)

peer.o: peer.c port.h ip-conv.c ip-conv.h peer_parser.c peer_parser.h

clean:
	rm -f peer *.o
