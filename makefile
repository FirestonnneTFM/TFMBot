CC = gcc
# maximize warning messages !
CFLAGS = -std=gnu99 -Wall -Wextra -Wpedantic -Wshadow -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Werror
TARGET = ./bin/remote
SOURCES = $(wildcard ./src/*.c)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

clean:
	rm -f $(OBJECTS)
