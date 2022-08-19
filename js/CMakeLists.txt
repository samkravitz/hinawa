set(SOURCES
	ast/ast.cc

	scanner.cc
	token.cc
)

set(HEADERS
	ast/ast.h
	ast/expr.h
	ast/stmt.h

	opcode.h
	scanner.h
	token_type.h
	token.h
	value.h
	vm.h
)

flex_target(JsLexer2 lex.ll lex.yy.cc)

add_executable(js main.cc ${SOURCES} ${HEADERS} ${FLEX_JsLexer2_OUTPUTS})
target_include_directories(js PUBLIC .)

target_sources(hinawa PUBLIC ${SOURCES} ${HEADERS})
