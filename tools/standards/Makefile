CC=g++
CFLAGS=-std=c++17 -Wall

all: StandardChecker

StandardChecker: StandardChecker.cpp
	$(CC) $(CFLAGS) -o StandardChecker StandardChecker.cpp

run: StandardChecker
	./StandardChecker ../../src

clean:
	rm -f StandardChecker
