# makefile for scheduling program        
CC = gcc
CFLAGS = -Wall -Wextra
SRCS = scheduler.c queue.c
TARGET = scheduler

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

