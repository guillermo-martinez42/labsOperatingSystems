CC = gcc
CFLAGS = -Wall -pthread -O2 -fsanitize=address
LDFLAGS = -lSDL2 -lSDL2_ttf -pthread

TARGET = parking_lot
SOURCES = parking_lot.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
