CXX = g++
CXXFLAGS = -std=c++2a -g -Wall -Wpedantic
SRC = \
	dom.cc \
	element.cc \
	node.cc \
	text.cc

OBJ = $(SRC:.cc=.o)

all: main

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f *.o
