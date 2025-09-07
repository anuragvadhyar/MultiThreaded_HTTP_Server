CC=clang
CFLAGS=-Wall -Wextra -g -Iinclude
LDFLAGS=-lpthread

SRCS=src/main.c src/map.c src/thread_pool.c src/html_formatting.c
TARGET=server

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) src/*.o