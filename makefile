TARGET = standard
CC = g++
CFLAGS = -Wall -Wextra -pedantic
LINES = 100

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $@.cpp -o $(TARGET)

test:
	$(CC) assignment_2_gen.cc -o generate
	./generate $(LINES) > test_$(LINES).txt	

clean:
	rm -rf *.txt $(TARGET) generate
