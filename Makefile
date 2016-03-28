TARGET = cpok
LIBS = -lncurses
CC = gcc
CFLAGS = -g -ansi -Wall -Werror -DLOGGING

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = main.o card.o tests.o pok.o player.o game.o ui.o settings.o ai.o
HEADERS = defs.h card.h tests.h pok.h player.h game.h ui.h settings.h ai.h ai_data.h obscene.h

%.o: %.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
		$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean:
		-rm -vf *.o
		-rm -vf $(TARGET)
