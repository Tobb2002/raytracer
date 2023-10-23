.PHONY: all

CC = g++
FLAGS = -g -c

BUILDDIR = build
SOURCEDIR = src

# tasks
all: compile checkstyle

compile: $(BUILDDIR) $(BUILDDIR)/raytracer

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

checkstyle:
	cpplint src/*.cpp

run: compile
	./$(BUILDDIR)/raytracer

# main
$(BUILDDIR)/raytracer: $(BUILDDIR)/raytracer.o
	g++ -o $(BUILDDIR)/raytracer $(BUILDDIR)/raytracer.o

$(BUILDDIR)/raytracer.o: $(SOURCEDIR)/raytracer.cpp
	g++ -c $(SOURCEDIR)/raytracer.cpp -o $(BUILDDIR)/raytracer.o

# modules