include ../config.mk

SRC = \
	dom.cc \
	element.cc \
	node.cc \
	text.cc

OBJ = $(SRC:.cc=.document.o)

ifdef FULL_BUILD
all: $(OBJ)
else
all: main
endif

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

%.document.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f *.o
