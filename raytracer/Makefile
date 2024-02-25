.PHONY: all

CC = clang++
FLAGS = -std=c++2a -Wall -O0 -g -fsanitize=address -L/usr/X11R6/lib -lm -lpthread -lX11 -fopenmp -pg

# tasks
all: compile

compile: bin/main

BUILDDIRS= obj bin

$(BUILDDIRS):
	mkdir -p $(BUILDDIRS)

clean:
	rm -rf $(BUILDDIRS)

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
	ffmpeg -framerate 25 -start_number 0 -i data/output/animation/sample%d.ppm -vcodec jpeg2000 data/output/finished/animation.avi

docs:
	doxygen doxygen.conf

compile_commands:
	compiledb --command-style -o src/compile_commands.json make

files = main ray triangle camera image mesh pointlight box plane scene object objloader object_factory transform bvh light sphere texture 

targets = $(addsuffix .o,$(addprefix obj/,$(files)))

# linke everything
bin/main: $(targets) | $(BUILDDIRS)
	$(CC) $(FLAGS) -o bin/main $(targets)

# main
obj/main.o: src/main.cpp src/scenes/ | $(BUILDDIRS)
	$(CC) $(FLAGS) -c src/main.cpp -o obj/main.o

# modules
obj/%.o: src/%.cpp src/%.hpp | $(BUILDDIRS)
	$(CC) $(FLAGS) -c $< -o $@

#objects
obj/%.o: src/objects/%.cpp src/objects/%.hpp | $(BUILDDIRS)
	$(CC) $(FLAGS) -c $< -o $@

# lib obj
obj/%.o: src/objects/lib/%.cpp src/objects/lib/%.hpp | $(BUILDDIRS)
	$(CC) $(FLAGS) -c $< -o $@
