include ../config.mk

LEX = flex -+

src = \
	parser.cc

OBJ = lex.yy.css.o $(SRC:.cc=.css.o)

ifdef FULL_BUILD
all: $(OBJ)
else
all: main
endif

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

lex.yy.css.o: lex.ll
	$(LEX) -o lex.yy.cc lex.ll
	$(CXX) $(CXXFLAGS) -c lex.yy.cc -o $@

%.css.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f lex.yy.cc
	rm -f *.o
