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
	cclint --filter=-build/include,-build/header_guard *.hpp *.cpp objects/*.hpp objects/*.cpp

run: compile
	./build/main

animation:
	ffmpeg -framerate 25 -start_number 0 -i data/output/animation/sample%d.ppm -vcodec mpeg4 data/output/finished/animation.avi

files = main ray triangle camera image mesh pointlight box plane scene object objloader

targets = $(addsuffix .o,$(addprefix build/,$(files)))

# linke everything
build/main: $(targets)
	$(CC) $(FLAGS) -o build/main $(targets)

# main
build/main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp -o build/main.o

# modules
build/%.o: %.cpp %.hpp
	$(CC) $(FLAGS) -c $< -o $@

#objects
build/%.o: objects/%.cpp objects/%.hpp
	$(CC) $(FLAGS) -c $< -o $@

# lib obj
build/objloader.o: objects/lib/objloader.cpp objects/lib/objloader.hpp
	$(CC) $(FLAGS) -c $< -o $@