.PHONY: all

CC = g++
FLAGS = -Wall -O -g -fsanitize=address

# tasks
all: compile

compile: build build/main

build:
	mkdir -p build

clean:
	rm -rf build

checkstyle:
	cclint --filter=-build/include,-build/header_guard *.h *.cpp objects/*.h objects/*.cpp

run: compile
	./build/main

# main
build/main: build/main.o build/ray.o build/triangle.o build/camera.o build/image.o build/mesh.o build/pointlight.o build/box.o build/plane.o
	$(CC) $(FLAGS) -o build/main build/main.o build/ray.o build/triangle.o build/camera.o build/image.o build/mesh.o build/pointlight.o build/box.o build/plane.o

build/main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp -o build/main.o

# modules
build/image.o: image.cpp image.h
	$(CC) $(FLAGS) -c image.cpp -o build/image.o

#objects
build/ray.o: objects/ray.cpp objects/ray.h
	$(CC) $(FLAGS) -c objects/ray.cpp -o build/ray.o

build/triangle.o: objects/triangle.cpp objects/triangle.h
	$(CC) $(FLAGS) -c objects/triangle.cpp -o build/triangle.o

build/camera.o: objects/camera.cpp objects/camera.h
	$(CC) $(FLAGS) -c objects/camera.cpp -o build/camera.o

build/mesh.o: objects/mesh.cpp objects/mesh.h
	$(CC) $(FLAGS) -c objects/mesh.cpp -o build/mesh.o

build/pointlight.o: objects/pointlight.cpp objects/pointlight.h
	$(CC) $(FLAGS) -c objects/pointlight.cpp -o build/pointlight.o

build/box.o: objects/box.cpp objects/box.h
	$(CC) $(FLAGS) -c objects/box.cpp -o build/box.o

build/plane.o: objects/plane.cpp objects/plane.hpp
	$(CC) $(FLAGS) -c objects/plane.cpp -o build/plane.o