CC = g++
COMMON_FLAGS = -std=gnu++17 -Wall -Werror -march=native -pthread
DBG_FLAGS = ${COMMON_FLAGS} -Og
RELEASE_FLAGS = ${COMMON_FLAGS} -O3
LD_FLAGS = -pthread

SRC_DIR := src
GENERATED_FILES := ${addprefix ${SRC_DIR}/, scanner.cpp parser.cpp }
SRC_FILES := ${wildcard ${SRC_DIR}/*.cpp}

DEPS = ${GENERATED_FILES} ${SRC_FILES}

TEST_DIR := tests

all: compiler_debug compiler_release

debug: compiler_debug

release: compiler_release

test_all: test_debug test_release

# So make doesn't try to build everytime.
debug_bin: ${DEPS} main.cpp
	${CC} ${DBG_FLAGS} $^ -o $@ ${LD_FLAGS}

compiler_debug: ${DEPS} main.cpp
	${CC} ${DBG_FLAGS} $^ -o debug_bin ${LD_FLAGS}

compiler_release: ${DEPS} main.cpp
	${CC} ${RELEASE_FLAGS} $^ -o release_bin ${LD_FLAGS}

test_debug: ${DEPS} ${TEST_DIR}/*.cpp
	${CC} ${DBG_FLAGS} $^ -o $@ ${LD_FLAGS}
	./$@
	rm -f $@

test_release: ${DEPS} ${TEST_DIR}/*.cpp
	${CC} ${RELEASE_FLAGS} $^ -o $@ ${LD_FLAGS}
	./$@
	rm -f $@

parser_graph: src/parser.ypp
	bison --defines=src/parser.h --verbose --graph -o src/parser.cpp src/parser.ypp
	dot -Tpng src/parser.dot -o parser.png

${GENERATED_FILES}: src/parser.ypp src/scanner.lex
	bison --defines=src/parser.h --verbose --graph -o src/parser.cpp src/parser.ypp
	flex -Cfe -o src/scanner.cpp src/scanner.lex

.PHONY: clean todo parser lexer
clean:
	rm -f debug_bin release_bin ${GENERATED_FILES} src/parser.dot src/parser.output parser.png prog.dot

todo:
	grep -Rn TODO

parser:
	bison --defines=src/parser.h --verbose --graph -o src/parser.cpp src/parser.ypp

lexer: 
	flex -Cfe -o src/scanner.cpp src/scanner.lex

# Formats source files in a consistent way using astyle. To install astyle, go to
# https://astyle.sourceforge.net/. I put it in ../tools/ relative to this directory.
# Formatting rules are in .astylerc
fmt: ${SRC_FILES} ${SRC_DIR}/*.h ${TEST_DIR}/*.cpp ${TEST_DIR}/*.h
	../tools/astyle --project=.astylerc $^

