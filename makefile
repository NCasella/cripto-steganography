CC = gcc
CFLAGS = -Wall -Wextra -I./src/include -g -fsanitize=address -lm

SRCS = main.c $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

TARGET = visualSSS

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean