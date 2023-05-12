CC = g++
COMMON_FLAGS = -std=gnu++17 -Wall -Werror -march=native -fsanitize=unreachable
DBG_FLAGS = ${COMMON_FLAGS} -Og
RELEASE_FLAGS = ${COMMON_FLAGS} -O3
LD_FLAGS = -pthread

SRC_DIR := src
GENERATED_FILES := ${addprefix ${SRC_DIR}/, scanner.cpp parser.cpp }
SRC_FILES := ${wildcard ${SRC_DIR}/*.cpp}
OBJ_FILES =  ${patsubst src/%.cpp, %.o, ${SRC_FILES}} 

DEPS = ${GENERATED_FILES} ${SRC_FILES}

TEST_DIR := tests

all: debug release

debug: debug_bin

release: release_bin

test_all: test_debug test_release

${OBJ_FILES}: %.o: src/%.cpp 
	${CC} ${COMMON_FLAGS} -c $^

main.o: main.cpp
	${CC} ${COMMON_FLAGS} -c $^

src/parser.cpp: src/parser.ypp
	bison --defines=src/parser.h --verbose --graph -o src/parser.cpp src/parser.ypp
	${CC} ${COMMON_FLAGS} -c $@

src/scanner.cpp: src/scanner.lex
	flex -Cfe -o src/scanner.cpp src/scanner.lex
	${CC} ${COMMON_FLAGS} -c $@

debug_bin: src/scanner.cpp src/parser.cpp main.o ${OBJ_FILES}
	${CC} ${COMMON_FLAGS} *.o -o $@ 

release_bin: src/scanner.cpp src/parser.cpp main.o ${OBJ_FILES} 
	${CC} ${RELEASE_FLAGS} *.o -o $@ 

test_debug: ${DEPS} ${TEST_DIR}/*.cpp
	${CC} -DDEBUG_TOKENS ${DBG_FLAGS} $^ -o $@ ${LD_FLAGS}
	./$@
	rm -f $@

test_release: ${DEPS} ${TEST_DIR}/*.cpp
	${CC} -DDEBUG_TOKENS ${RELEASE_FLAGS} $^ -o $@ ${LD_FLAGS}
	./$@
	rm -f $@

parser_graph: src/parser.ypp
	bison --defines=src/parser.h --verbose --graph -o src/parser.cpp src/parser.ypp
	dot -Tpng src/parser.dot -o parser.png

.PHONY: clean todo 
clean:
	rm -f debug_bin release_bin ${GENERATED_FILES} *.o src/scanner.dot src/scanner.output src/parser.dot src/parser.output parser.png prog.dot

todo:
	grep -Rn TODO

# Formats source files in a consistent way using astyle. To install astyle, go to
# https://astyle.sourceforge.net/. I put it in ../tools/ relative to this directory.
# Formatting rules are in .astylerc
fmt: ${SRC_FILES} ${SRC_DIR}/*.h ${TEST_DIR}/*.cpp ${TEST_DIR}/*.h
	../tools/astyle --project=.astylerc $^

