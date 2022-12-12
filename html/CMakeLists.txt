set(SOURCES
	parser.cc
	token.cc
	tokenizer.cc
)

set(HEADERS
	parser.h
	token.h
	tokenizer.h
)

target_include_directories(hinawa PRIVATE ..)
target_sources(hinawa PRIVATE ${SOURCES} ${HEADERS})
