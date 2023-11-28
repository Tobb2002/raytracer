.PHONY: all

CC = g++
FLAGS = -Wall -O -g -fsanitize=address

# tasks
all: compile

compile: directory bin/main

directory:
	mkdir -p obj
	mkdir -p bin

clean:
	rm -rf obj/* bin/*

# recursive wildcard function
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# get all source files
source_cpp = $(call rwildcard, src, *.cpp)
source_hpp = $(call rwildcard, src, *.hpp)

checkstyle:
	cclint --filter=-build/include,-build/header_guard $(source_cpp) $(source_hpp)

run: compile
	./bin/main

animation:
	ffmpeg -framerate 25 -start_number 0 -i data/output/animation/sample%d.ppm -vcodec mpeg4 data/output/finished/animation.avi

docs:
	doxygen doxygen.conf

files = main ray triangle camera image mesh pointlight box plane scene object objloader object_factory

targets = $(addsuffix .o,$(addprefix obj/,$(files)))

# linke everything
bin/main: $(targets)
	$(CC) $(FLAGS) -o bin/main $(targets)

# main
obj/main.o: src/main.cpp
	$(CC) $(FLAGS) -c src/main.cpp -o obj/main.o

# modules
obj/%.o: src/%.cpp src/%.hpp
	$(CC) $(FLAGS) -c $< -o $@

#objects
obj/%.o: src/objects/%.cpp src/objects/%.hpp
	$(CC) $(FLAGS) -c $< -o $@

# lib obj
obj/objloader.o: src/objects/lib/objloader.cpp src/objects/lib/objloader.hpp
	$(CC) $(FLAGS) -c $< -o $@
