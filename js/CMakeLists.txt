set(SOURCES
	scanner.cc
	token.cc
)

set(HEADERS
	opcode.h
	scanner.h
	token_type.h
	token.h
	value.h
	vm.h
)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
