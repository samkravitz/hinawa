set(SOURCES
	css/selector_test.cc
)

target_include_directories(hinawa.test PRIVATE ..)
target_sources(hinawa.test PUBLIC ${SOURCES})
