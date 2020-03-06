CC=gcc
CFLAGS=-c -Wall
LIBS=-lblkid
SOURCES=$(wildcard *.c)

mount.out: $(SOURCES:.c=.o)
	$(CC) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm *.o mount.out
