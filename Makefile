CC=gcc
CFLAGS=-c -Wall
SOURCES=$(wildcard *.c)

abmount.out: $(SOURCES:.c=.o)
	$(CC) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm *.o abmount.out
