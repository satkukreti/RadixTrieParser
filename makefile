TARGET = standard
TARGET2 = optimized
CC = g++
CFLAGS = -Wall -Wextra -pedantic
LINES = 10
SRC = $(TARGET).cpp
SRC2 = $(TARGET2).cpp
GEN_SRC = assignment_2_gen.cc

all: $(TARGET) $(TARGET2)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -c $^ -o $@.o
	$(CC) $@.o -o $@

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -c $^ -o $@.o
	$(CC) $@.o -o $@

test:
	$(CC) $(GEN_SRC) -o generate
	./generate $(LINES) > test_$(LINES).txt

clean:
	rm -rf *.txt $(TARGET) $(TARGET2) generate *.o
