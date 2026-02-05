CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lgdbm
TARGET = reservation_system
SOURCES = main.c reservation_functions.c seatmap.c
OBJECTS = $(SOURCES:.c=.o)
DATA_FILES = reservation_data.gdbm seatmap_data.gdbm

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(DATA_FILES)

test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test
