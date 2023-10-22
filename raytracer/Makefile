.PHONY: all

CC = g++
FLAGS = -g -c

BUILDDIR = build
SOURCEDIR = src

# tasks
all: build checkstyle

build: dir raytracer

dir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

checkstyle:
	echo "\nSTYLECHECKER:"
	cpplint src/*.cpp

run: build
	./$(BUILDDIR)/raytracer

# main
raytracer: $(BUILDDIR)/raytracer.o
	g++ -o $(BUILDDIR)/raytracer $(BUILDDIR)/raytracer.o

$(BUILDDIR)/raytracer.o: $(SOURCEDIR)/raytracer.cpp
	g++ -c $(SOURCEDIR)/raytracer.cpp -o $(BUILDDIR)/raytracer.o

# modules