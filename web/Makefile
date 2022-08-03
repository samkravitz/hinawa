include ../config.mk

OBJ = $(SRC:.cc=.web.o)

ifdef FULL_BUILD
all: $(OBJ)
else
all: main
endif

main: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

%.web.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f main
	rm -f *.o
