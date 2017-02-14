
TARGET = modbus_player
all: $(TARGET)

HEADERS = $(wildcard *.h ../sfcore/include/*.h ../sfcore/include/sfcore/*.h ../sfio/include/*.h ../sfio/include/sfio/*.h)
SOURCES = $(filter-out $(wildcard *_win32.c),$(wildcard *.c))
OBJECTS = $(SOURCES:.c=.o)

GIO_CFLAGS != pkg-config --cflags gio-2.0
GIO_LIBS != pkg-config --libs gio-2.0

CC = gcc
CC_FLAGS = -Wall -g -I$(CURDIR)/../sfcore/include -I$(CURDIR)/../sfio/include $(GIO_CFLAGS)

$(TARGET): $(OBJECTS)
	$(MAKE) -C $(CURDIR)/../sfcore
	$(MAKE) -C $(CURDIR)/../sfio
	$(CC) -o $@ $^ -L$(CURDIR)/../sfcore -lsfcore -L$(CURDIR)/../sfio -lsfio $(GIO_LIBS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $(CC_FLAGS) $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)
	$(MAKE) -C $(CURDIR)/../sfio clean
	$(MAKE) -C $(CURDIR)/../sfcore clean

print-%:
	@echo '$*=$($*)'
