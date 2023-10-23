.PHONY: all

CC = g++
FLAGS = -Wall

# tasks
all: compile checkstyle

compile: build build/raytracer

build:
	mkdir -p build

clean:
	rm -rf build

checkstyle:
	cpplint --filter=-build/include *.cpp

run: compile
	./build/raytracer

# main
build/raytracer: build/raytracer.o build/filehandler.o
	$(CC) $(FLAGS) -o build/raytracer build/raytracer.o build/filehandler.o

build/raytracer.o: raytracer.cpp
	$(CC) $(FLAGS) -c raytracer.cpp -o build/raytracer.o

# modules
build/filehandler.o: filehandler.cpp filehandler.h
	$(CC) $(FLAGS) -c filehandler.cpp -o build/filehandler.o