CC = gcc
CFLAGS = -Wall -pthread -O2
LDFLAGS = -pthread

TARGET = log_processor
SOURCES = main_linux.c log_processor.c
HEADERS = log_processor.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
