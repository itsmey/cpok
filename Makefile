TARGET = cpok
LIBS = -lncurses
CC = gcc
CFLAGS = -g -ansi -Wall -Werror -DTESTING

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = main.o card.o tests.o pok.o player.o game.o ui.o
HEADERS = defs.h card.h tests.h pok.h player.h game.h ui.h obscene.h

%.o: %.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
		$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean:
		-rm -vf *.o
		-rm -vf $(TARGET)