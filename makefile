TARGET = standard
CC = g++
CFLAGS = -Wall -Wextra -pedantic
LINES = 100
SRC = $(TARGET).cpp
GEN_SRC = assignment_2_gen.cc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -c $^ -o $@.o
	$(CC) $@.o -o $@

test:
	$(CC) $(GEN_SRC) -o generate
	./generate $(LINES) > test_$(LINES).txt

clean:
	rm -rf *.txt $(TARGET) generate *.o
