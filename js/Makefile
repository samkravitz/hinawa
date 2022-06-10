CXX = g++
CXXFLAGS = -std=c++2a -g -Wall -Wpedantic
LEX = flex -+
SRC = \
	scanner.cc \
	token.cc \

OBJ = lex.yy.o $(SRC:.cc=.o)

all: main

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

lex.yy.o: lex.ll
	$(LEX) -o lex.yy.cc lex.ll
	$(CXX) $(CXXFLAGS) -c lex.yy.cc -o $@

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f lex.yy.cc
	rm -f *.o
