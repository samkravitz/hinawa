include ../config.mk

LEX = flex -+
SRC = \
	scanner.cc \
	token.cc \

OBJ = lex.yy.js.o $(SRC:.cc=.js.o)

ifdef FULL_BUILD
all: $(OBJ)
else
all: main
endif

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

lex.yy.js.o: lex.ll
	$(LEX) -o lex.yy.cc lex.ll
	$(CXX) $(CXXFLAGS) -c lex.yy.cc -o $@

%.js.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f lex.yy.cc
	rm -f *.o
