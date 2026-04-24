CC = gcc
CFLAGS = -Wall -pthread -O2 -fsanitize=address
LDFLAGS = -pthread

TARGET = scheduling
SOURCES = main.c process/process.c scheduler/scheduler.c scheduler/queue.c logs/logger.c
HEADERS = process/process.h scheduler/queue.h logs/logger.h

CC = gcc

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
