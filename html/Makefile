include ../config.mk

SRC = \
	parser.cc \
	token.cc \
	tokenizer.cc

INCLUDE = -I ..
CXXFLAGS += $(INCLUDE)

OBJ = $(SRC:.cc=.html.o)

ifdef FULL_BUILD
all: $(OBJ)
else
all: main
endif

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

%.html.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f *.o
