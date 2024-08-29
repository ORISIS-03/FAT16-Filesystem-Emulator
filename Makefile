CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -D_FILE_OFFSET_BITS=64
LDFLAGS = `pkg-config --libs gtk+-3.0` -lfuse

main: main.o
	$(CC) -o main main.o $(LDFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

clean:
	rm -f main main.o
