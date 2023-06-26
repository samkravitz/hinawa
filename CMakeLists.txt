cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS "-Wall -Wpedantic")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options("-g")
	add_compile_options("-DDEBUG")
	add_compile_options(-DHINAWA_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_options("-O2")
endif()

project(hinawa)

find_package(CURL REQUIRED)
find_package(fmt REQUIRED)
find_package(FLEX REQUIRED)
find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

flex_target(JsLexer js/lex.ll js/lex.yy.cc)

file(STRINGS "skia_path.txt" Skia_INSTALL_DIR)
set(Skia_BUILD_DIR ${Skia_INSTALL_DIR} CACHE PATH "Skia build directory")

find_path(Skia_INCLUDE_DIR
	PATH ${Skia_BUILD_DIR}/include
)

find_library(Skia_LIBRARY
	skia
	PATH ${Skia_BUILD_DIR}/out/Static
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Skia DEFAULT_MSG Skia_LIBRARY Skia_INCLUDE_DIR)
MARK_AS_ADVANCED(Skia_INCLUDE_DIR Skia_LIBRARY)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

qt_add_executable(hinawa
    main.cc
	${FLEX_CssLexer_OUTPUTS}
	${FLEX_JsLexer_OUTPUTS}
)

add_executable(hinawa.test)
target_sources(hinawa.test PRIVATE ${FLEX_CssLexer_OUTPUTS} ${FLEX_JsLexer_OUTPUTS})
target_include_directories(hinawa PRIVATE css js)
target_include_directories(hinawa.test PRIVATE css js)

add_subdirectory(css)
add_subdirectory(browser)
add_subdirectory(document)
add_subdirectory(html)
add_subdirectory(js)
add_subdirectory(layout)
add_subdirectory(test)
add_subdirectory(web)

target_link_libraries(hinawa
	PRIVATE
	curl
	sfml-graphics
	sfml-window
	sfml-system
	fmt::fmt
	nlohmann_json::nlohmann_json
	Qt6::Widgets
	Qt6::Core
	Qt6::Gui
	${Skia_LIBRARY}
	${FREETYPE_LIBRARIES}
	Fontconfig::Fontconfig
)

enable_testing()

target_link_libraries(
	hinawa.test
 	GTest::gtest_main
	curl
	sfml-graphics
	sfml-window
	sfml-system
	fmt::fmt
	nlohmann_json::nlohmann_json
	Qt6::Widgets
	Qt6::Core
	Qt6::Gui
	${Skia_LIBRARY}
	${FREETYPE_LIBRARIES}
	Fontconfig::Fontconfig
)

include(GoogleTest)
gtest_discover_tests(hinawa.test)
