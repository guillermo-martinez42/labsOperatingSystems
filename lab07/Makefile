CC = gcc
CFLAGS = -Wall -pthread -O2 -fsanitize=address
LDFLAGS = -pthread

TARGET = zombie_bridge
SOURCES = zombie_bridge.c
HEADERS = zombie_bridge.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
