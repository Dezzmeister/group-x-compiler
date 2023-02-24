CC = g++
COMMON_FLAGS = -std=gnu++17 -Wall -Werror -march=native
DBG_FLAGS = ${COMMON_FLAGS} -Og
RELEASE_FLAGS = ${COMMON_FLAGS} -O3
LD_FLAGS = -ll

SRC_DIR := src
GENERATED_FILES := ${addprefix ${SRC_DIR}/, scanner.cpp}

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

src/scanner.cpp: src/scanner.lex
	lex -o src/scanner.cpp $^

clean: debug_bin release_bin ${GENERATED_FILES}
	rm -f $^

