CC=gcc
CFLAGS=-c -Wall
LIBS=-lblkid
SOURCES=$(wildcard *.c)

abmount.out: $(SOURCES:.c=.o)
	$(CC) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm *.o abmount.out
