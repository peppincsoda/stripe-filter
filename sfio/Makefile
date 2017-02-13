
TARGET = libsfio.so
all: $(TARGET)

HEADERS = $(wildcard include/*.h include/sfio/*.h ../sfcore/include/*.h ../sfcore/include/sfcore/*.h)
SOURCES = $(filter-out $(wildcard *_win32.c),$(wildcard *.c))
OBJECTS = $(SOURCES:.c=.o)

GIO_CFLAGS != pkg-config --cflags gio-2.0
GIO_LIBS != pkg-config --libs gio-2.0

CC = gcc
CC_FLAGS = -Wall -g -DSFIO_COMPILATION -I$(CURDIR)/include -I$(CURDIR)/../sfcore/include $(GIO_CFLAGS)

$(TARGET): $(OBJECTS)
	$(MAKE) -C $(CURDIR)/../sfcore
	$(CC) -shared -o $@ -fPIC $^ -L$(CURDIR)/../sfcore -lsfcore $(GIO_LIBS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $(CC_FLAGS) $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)
	$(MAKE) -C $(CURDIR)/../sfcore clean

print-%:
	@echo '$*=$($*)'
