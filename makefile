CC = gcc
CFLAGS = -Wall -Wpedantic -O2
TARGET = ./bin/remote
SOURCES = $(wildcard ./src/*.c)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

clean:
	rm -f $(OBJECTS)
