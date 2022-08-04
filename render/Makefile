include ../config.mk

SRC = \
	window.cc

OBJ = $(SRC:.cc=.render.o)

ifdef FULL_BUILD
all: $(OBJ)
else
all: main
endif

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ) $(LIB)

%.render.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f *.o
