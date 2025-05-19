CC = gcc
CFLAGS = -Wall -Wextra -I./src/include

SRCS = main.c $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

TARGET = bmpsteg.out

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean