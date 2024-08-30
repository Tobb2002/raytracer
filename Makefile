.PHONY: all

#BUILD=debug

CC = clang++
FLAGS = -std=c++2a -W #-fno-elide-constructors
LINKER_FLAGS = -lm -lX11 -lprofiler -ltbb

ifeq ($(BUILD),debug)   
# "Debug" build - no optimization, and debugging symbols
FLAGS += -fsanitize=address -O0 -g -pg
OBJ_DIR = obj/debug
else
# "Release" build - optimization, and no debug symbols
FLAGS += -O2 -DNDEBUG
OBJ_DIR = obj/release
endif

# tasks
all: compile

compile: bin/main

BUILDDIRS= $(OBJ_DIR) bin

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
	cclint --filter=-build/include,-build/header_guard,-build/c++11 $(source_cpp) $(source_hpp)

format:
	clang-format -i -style=google $(source_cpp) $(source_hpp)

stats:
	cloc $(source_cpp) $(source_hpp)

run: compile
	./bin/main
	magick data/output/out.ppm data/output/out.png

profile_valgrind: ./bin/main
	valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes bin/main
profile_gperf: ./bin/main
	env CPUPROFILE=main.prof ./bin/main
	pprof ./bin/main ./main.prof
animation:
	ffmpeg -framerate 25 -start_number 0 -i data/output/animation/sample%d.ppm -vcodec jpeg2000 data/output/finished/animation.avi

docs:
	doxygen doxygen.conf

compile_commands:
	compiledb --command-style -o src/compile_commands.json make

files = main ray triangle camera image mesh pointlight box plane scene object objloader object_factory transform bvh light sphere texture bvh_tree sah lbvh morton uniform_grid

targets = $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(files)))

# linke everything
bin/main: $(targets) | $(BUILDDIRS)
	$(CC) $(FLAGS) $(LINKER_FLAGS) -o bin/main $(targets)

# main
$(OBJ_DIR)/main.o: src/main.cpp src/scenes/ | $(BUILDDIRS)
	$(CC) $(FLAGS) -c src/main.cpp -o $(OBJ_DIR)/main.o

# modules
$(OBJ_DIR)/%.o: src/%.cpp src/%.hpp | $(BUILDDIRS)
	$(CC) $(FLAGS) -c $< -o $@

#objects
$(OBJ_DIR)/%.o: src/objects/%.cpp src/objects/%.hpp | $(BUILDDIRS)
	$(CC) $(FLAGS) -c $< -o $@

# lib obj
$(OBJ_DIR)/%.o: src/objects/lib/%.cpp src/objects/lib/%.hpp | $(BUILDDIRS)
	$(CC) $(FLAGS) -c $< -o $@
