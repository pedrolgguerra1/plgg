CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/main.c src/parser.c src/task.c src/executor.c src/job.c
TARGET = processflow

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean