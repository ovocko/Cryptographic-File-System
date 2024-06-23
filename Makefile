CC=gcc
CFLAGS=-Wall `pkg-config fuse --cflags` -I. -D_FILE_OFFSET_BITS=64
LIBS=`pkg-config fuse --libs` -lssl -lcrypto

all: cryptofs

cryptofs: cryptofs.o encryption.o
	$(CC) -o cryptofs cryptofs.o encryption.o $(LIBS)

cryptofs.o: cryptofs.c encryption.h
	$(CC) $(CFLAGS) -c cryptofs.c

encryption.o: encryption.c encryption.h
	$(CC) $(CFLAGS) -c encryption.c

clean:
	rm -f *.o cryptofs

