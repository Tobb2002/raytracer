.PHONY: all

CC = g++
FLAGS = -Wall

# tasks
all: compile

compile: build build/raytracer

build:
	mkdir -p build

clean:
	rm -rf build

checkstyle:
	cclint --filter=-build/include,-build/header_guard *.cpp *.h

run: compile
	./build/raytracer

# main
build/raytracer: build/raytracer.o build/ray.o build/triangle.o build/camera.o build/image.o build/mesh.o
	$(CC) $(FLAGS) -o build/raytracer build/raytracer.o build/ray.o build/triangle.o build/camera.o build/image.o build/mesh.o

build/raytracer.o: raytracer.cpp
	$(CC) $(FLAGS) -c raytracer.cpp -o build/raytracer.o

# modules
build/ray.o: ray.cpp ray.h
	$(CC) $(FLAGS) -c ray.cpp -o build/ray.o

build/triangle.o: triangle.cpp triangle.h
	$(CC) $(FLAGS) -c triangle.cpp -o build/triangle.o

build/camera.o: camera.cpp camera.h
	$(CC) $(FLAGS) -c camera.cpp -o build/camera.o

build/image.o: image.cpp image.h
	$(CC) $(FLAGS) -c image.cpp -o build/image.o

build/mesh.o: mesh.cpp mesh.h
	$(CC) $(FLAGS) -c mesh.cpp -o build/mesh.o