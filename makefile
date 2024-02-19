TARGET = standard
CC = g++
CFLAGS = -Wall -Wextra -pedantic

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $@.cpp -o $(TARGET)

clean:
	rm -rf $(TARGET)
