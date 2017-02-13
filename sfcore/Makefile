
TARGET = libsfcore.so
all: $(TARGET)

HEADERS = $(wildcard include/*.h include/sfcore/*.h)
SOURCES = $(filter-out $(wildcard *_win32.c),$(wildcard *.c))
OBJECTS = $(SOURCES:.c=.o)

GIO_CFLAGS != pkg-config --cflags gio-2.0
GIO_LIBS != pkg-config --libs gio-2.0

CC = gcc
CC_FLAGS = -Wall -g -DSFCORE_COMPILATION -I$(CURDIR)/include $(GIO_CFLAGS)

$(TARGET): $(OBJECTS)
	$(CC) -shared -o $@ -fPIC $^ $(GIO_LIBS) -lm -lrt

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $(CC_FLAGS) $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)

print-%:
	@echo '$*=$($*)'
