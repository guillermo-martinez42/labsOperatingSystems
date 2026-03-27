CC = gcc
CFLAGS = -Wall -pthread -O2 -fsanitize=address
LDFLAGS = -pthread

TARGET = zombie_bridge
SOURCES = zombie_bridge.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
