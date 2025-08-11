set(CMAKE_CXX_STANDARD 20)          
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_LIB
	simulator.cpp
	parser.cpp
	lexer.cpp
	LanguageErrors.cpp
)

add_library(definitions_lib STATIC ${SOURCE_LIB})

target_include_directories(definitions_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)