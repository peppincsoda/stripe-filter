
all: grabc

EXEC = grabc
SOURCES = $(filter-out $(wildcard *_win32.c),$(wildcard *.c))
OBJECTS = $(SOURCES:.c=.o)

GIO_CFLAGS != pkg-config --cflags gio-2.0
GIO_LIBS != pkg-config --libs gio-2.0

PYLON_CFLAGS = -I/opt/pylon5/include
PYLON_LIBS = -Wl,--enable-new-dtags -Wl,-rpath,/opt/pylon5/lib -L/opt/pylon5/lib -lpylonc

CC = gcc
CC_FLAGS = -Wall -g $(PYLON_CFLAGS) $(GIO_CFLAGS)

#OBJ_DIR = obj

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ $(GIO_LIBS) $(PYLON_LIBS) -lm -lrt

%.o: %.c
	$(CC) -c -o $@ $(CC_FLAGS) $<

.PHONY: clean
clean:
	rm -f $(EXEC) $(OBJECTS)


print-%:
	@echo '$*=$($*)'
