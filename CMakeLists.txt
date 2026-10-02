# debug
if(OFF)
	set(CMAKE_CXX_COMPILER clang++)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	set(CMAKE_BUILD_TYPE Debug)
endif()


cmake_minimum_required(VERSION 3.5..4.4)
project(table CXX)

set(CXX_REQUIRED_FEATURE cxx_std_23)

add_library(core SHARED)
target_compile_features(core PRIVATE ${CXX_REQUIRED_FEATURE})

target_include_directories(core PRIVATE src/cppcore)
target_sources(core PRIVATE src/cppcore/core.impl.cpp)
install(TARGETS core LIBRARY DESTINATION "${CMAKE_SOURCE_DIR}/lib")


function(add_bin_tool name src)
	add_executable(${name})
	target_compile_features(${name} PRIVATE ${CXX_REQUIRED_FEATURE})

	target_sources(${name} PRIVATE ${src})
	target_include_directories(${name} PUBLIC src/cppcore)

	target_link_libraries(${name} PRIVATE core)

	set_target_properties(${name} PROPERTIES INSTALL_RPATH "$ORIGIN:$ORIGIN/lib")
	install(TARGETS ${name} RUNTIME DESTINATION "${CMAKE_SOURCE_DIR}/bin")

endfunction()


add_bin_tool(cppt src/binsrc/cppt.cpp)

add_bin_tool(encoder src/binsrc/encoder.cpp)
add_bin_tool(encoder-with-filter src/binsrc/encoder-with-filter.cpp)

add_bin_tool(get-freq src/binsrc/get-freq.cpp)
add_bin_tool(get-freq-with-filter src/binsrc/get-freq-with-filter.cpp)

add_bin_tool(codec-set-intersect src/binsrc/codec-set-intersect.cpp)
add_bin_tool(codec-set-unite src/binsrc/codec-set-unite.cpp)
add_bin_tool(codec-set-difference src/binsrc/codec-set-difference.cpp)

add_bin_tool(word-set-intersect src/binsrc/word-set-intersect.cpp)
add_bin_tool(word-set-unite src/binsrc/word-set-unite.cpp)
add_bin_tool(word-set-difference src/binsrc/word-set-difference.cpp)

add_bin_tool(word-codec-set-intersect src/binsrc/word-codec-set-intersect.cpp)
add_bin_tool(word-codec-set-unite src/binsrc/word-codec-set-unite.cpp)
add_bin_tool(word-codec-set-difference src/binsrc/word-codec-set-difference.cpp)

add_bin_tool(pinyin-normalize src/binsrc/pinyin-normalize.cpp)
add_bin_tool(pinyin-add-aux src/binsrc/pinyin-add-aux.cpp)

add_bin_tool(sort-freq src/binsrc/sort-freq.cpp)

add_bin_tool(sort-word src/binsrc/sort-word.cpp)
add_bin_tool(sort-word-codec src/binsrc/sort-word-codec.cpp)

add_bin_tool(sort-codec-seq src/binsrc/sort-codec-seq.cpp)
add_bin_tool(sort-codec-seq-wbfilter src/binsrc/sort-codec-seq-wbfilter.cpp)

add_bin_tool(make-seq-from-freq src/binsrc/make-seq-from-freq.cpp)
add_bin_tool(make-seq-from-freq-wbfilter 
	src/binsrc/make-seq-from-freq-wbfilter.cpp)

add_bin_tool(fmt-to-scheme src/binsrc/fmt-to-scheme.cpp)

add_bin_tool(unique-line src/binsrc/unique-line.cpp)
add_bin_tool(unique-line-keep-seq src/binsrc/unique-line-keep-seq.cpp)

add_bin_tool(unique-major-voice src/binsrc/unique-major-voice.cpp)

add_bin_tool(sort-num-selection src/binsrc/sort-num-selection.cpp)

add_bin_tool(find-easyword-3 src/binsrc/find-easyword-3.cpp)
add_bin_tool(find-easyword-4 src/binsrc/find-easyword-4.cpp)

add_bin_tool(hanzi-to-pinyin src/binsrc/hanzi-to-pinyin.cpp)
