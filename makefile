CC = gcc
# maximize warning messages !
CFLAGS = -std=gnu99 -Wall -Wextra -Wpedantic -Wshadow -Wcast-qual	\
-Wstrict-prototypes -Wmissing-prototypes -Werror -Wno-multichar -O2
TARGET = ./bin/remote
LDLIBS = -lpthread -lz
SOURCES = $(wildcard ./src/*.c)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDLIBS)

clean:
	rm -f $(OBJECTS)
