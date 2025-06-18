CC=/usr/bin/cc

CFLAGS= -std=iso9899:1999 -pedantic -pedantic-errors \
	-g -O0 \
	-fno-fast-math -fno-builtin -fno-unsafe-math-optimizations

CPPFLAGS= -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
	-D_XOPEN_SOURCE=600

LDIR=/usr/local/lib
IDIR=/usr/local/include

LIBS=-lX11 -lm

SRCS = src/main.c  src/player.c  src/util.c  src/wav.c
OBJS = src/main.o  src/player.o  src/util.o  src/wav.o

.SUFFIXES: .c .o
.c.o:
	$(CC) -o $@ -c $< $(CFLAGS) -I./include -I$(IDIR) $(CPPFLAGS)

XOpenPlayer: $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(CPPFLAGS) -L$(LDIR) $(LIBS)

clean:
	rm -f $(OBJS) XOpenPlayer