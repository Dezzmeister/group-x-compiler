CC = g++
COMMON_FLAGS = -std=gnu++17 -Wall -Werror -march=native
DBG_FLAGS = ${COMMON_FLAGS} -O -g
RELEASE_FLAGS = ${COMMON_FLAGS} -O3
LD_FLAGS = -ll

SRC_DIR := src
GENERATED_FILES := ${addprefix ${SRC_DIR}/, scanner.cpp parser.cpp parser.h}

DEPS = ${GENERATED_FILES} ${wildcard ${SRC_DIR}/*.h} ${wildcard ${SRC_DIR}/*.cpp}

TEST_DIR := tests

all: compiler_debug compiler_release

debug: compiler_debug

release: compiler_release

test_all: test_debug test_release

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

${GENERATED_FILES}: src/parser.ypp src/scanner.lex
	bison  --defines=src/parser.h -o src/parser.cpp src/parser.ypp
	flex -Cfe -o src/scanner.cpp src/scanner.lex

.PHONY: clean
clean:
	rm -f debug_bin release_bin ${GENERATED_FILES}

