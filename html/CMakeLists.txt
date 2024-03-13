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

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
