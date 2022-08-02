CXX = g++
CXXFLAGS = -std=c++2a -g -Wall -Wpedantic
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

OBJ = $(SRC:.cc=.o)

all: main

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ) $(LIBS)

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f *.o
