set(SOURCES
	css/selector_test.cc

	js/value_test.cc
)

target_sources(hinawa.test PUBLIC ${SOURCES} ${HEADERS})
