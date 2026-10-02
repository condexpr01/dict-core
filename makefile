.PHONY: all compile build install run clean

all: install

compile:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G Ninja

build: compile
	cmake --build build --config Release

install: build
	cmake --install build

run:
	./bin/cppt

clean: 
	-cmake -E rm -rf build
	-cmake -E rm -rf ./lib
	-cmake -E rm -rf ./.cache
	
	-cmake -E rm ./bin/unique-line
	-cmake -E rm ./bin/unique-line-keep-seq
	-cmake -E rm ./bin/unique-major-voice
	
	-cmake -E rm ./bin/encoder
	-cmake -E rm ./bin/encoder-with-filter
	
	-cmake -E rm ./bin/get-freq
	-cmake -E rm ./bin/get-freq-with-filter
	
	-cmake -E rm ./bin/word-set-intersect
	-cmake -E rm ./bin/word-set-unite
	-cmake -E rm ./bin/word-set-difference
	
	-cmake -E rm ./bin/word-codec-set-intersect
	-cmake -E rm ./bin/word-codec-set-unite
	-cmake -E rm ./bin/word-codec-set-difference
	
	-cmake -E rm ./bin/codec-set-intersect
	-cmake -E rm ./bin/codec-set-unite
	-cmake -E rm ./bin/codec-set-difference
	
	-cmake -E rm ./bin/pinyin-normalize
	-cmake -E rm ./bin/pinyin-add-aux
	
	-cmake -E rm ./bin/sort-freq
	-cmake -E rm ./bin/sort-word
	-cmake -E rm ./bin/sort-word-codec
	
	-cmake -E rm ./bin/sort-codec-seq
	-cmake -E rm ./bin/sort-codec-seq-wbfilter
	
	-cmake -E rm ./bin/sort-num-selection
	
	-cmake -E rm ./bin/find-easyword-3
	-cmake -E rm ./bin/find-easyword-4
	
	-cmake -E rm ./bin/make-seq-from-freq
	-cmake -E rm ./bin/make-seq-from-freq-wbfilter
	
	-cmake -E rm ./bin/fmt-to-scheme
	
	-cmake -E rm ./bin/hanzi-to-pinyin
	
	-cmake -E rm ./bin/cppt
	
	-cmake -E rm -rf ./src/.cache
	-cmake -E rm -rf ./src/binsrc/.cache
	
	-cmake -E rm -rf ./src/cppcore/.cache


