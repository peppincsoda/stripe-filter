
TARGET = grabc
all: $(TARGET)

HEADERS = $(wildcard *.h ../sfcore/include/*.h ../sfcore/include/sfcore/*.h ../sfio/include/*.h ../sfio/include/sfio/*.h)
SOURCES = $(filter-out $(wildcard *_win32.c),$(wildcard *.c))
OBJECTS = $(SOURCES:.c=.o)

GIO_CFLAGS != pkg-config --cflags gio-2.0
GIO_LIBS != pkg-config --libs gio-2.0

PYLON_CFLAGS = -I/opt/pylon5/include
PYLON_LIBS = -Wl,--enable-new-dtags -Wl,-rpath,/opt/pylon5/lib -L/opt/pylon5/lib -lpylonc

CC = gcc
CC_FLAGS = -Wall -g -I$(CURDIR)/../sfcore/include -I$(CURDIR)/../sfio/include $(PYLON_CFLAGS) $(GIO_CFLAGS)

$(TARGET): $(OBJECTS)
	$(MAKE) -C $(CURDIR)/../sfcore
	$(MAKE) -C $(CURDIR)/../sfio
	$(CC) -o $@ $^ -L$(CURDIR)/../sfcore -lsfcore -L$(CURDIR)/../sfio -lsfio $(GIO_LIBS) $(PYLON_LIBS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $(CC_FLAGS) $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)
	$(MAKE) -C $(CURDIR)/../sfio clean
	$(MAKE) -C $(CURDIR)/../sfcore clean

print-%:
	@echo '$*=$($*)'
