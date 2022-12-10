cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-DDEBUG -g -Wall -Wpedantic")

project(hinawa)

find_package(FLEX REQUIRED)
flex_target(CssLexer css/lex.ll css/lex.yy.cc)
flex_target(JsLexer js/lex.ll js/lex.yy.cc)

add_executable(hinawa ${FLEX_CssLexer_OUTPUTS} ${FLEX_JsLexer_OUTPUTS})
target_sources(hinawa PRIVATE main.cc)
target_include_directories(hinawa PRIVATE css js)

add_subdirectory(css)
add_subdirectory(document)
add_subdirectory(html)
add_subdirectory(js)
add_subdirectory(layout)
add_subdirectory(render)

target_link_libraries(hinawa sfml-graphics sfml-window sfml-system)
