include config.mk

OBJ = $(wildcard document/*.o html/*.o js/*.o render/*.o web/*.o)

MODULES = \
	document \
	html \
	js \
	render \
	web

export FULL_BUILD = 1

all: hinawa

modules:
	for module in $(MODULES); do \
		echo building $$module...; \
		$(MAKE) -C $$module; \
	done

hinawa: main.cc modules
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ) $(LIB)

.PHONY: clean
clean:
	rm -rf hinawa
	for module in $(MODULES); do \
		echo cleaning $$module...; \
		$(MAKE) -C $$module clean; \
	done

.SILENT:
