COMMON_FLAGS = -Wall -Werror -std=gnu++17 -march=native -fsanitize=unreachable
SRCS := $(wildcard src/*.cpp)

${DEPS_DIR}

SRC_DIR      := src
DEPS_DIR     := _deps
TEST_DIR     := tests
RELEASE_DIR  := _release
DEBUG_DIR    := _debug

GENERATED_FILES := ${addprefix ${SRC_DIR}/,scanner.cpp parser.cpp}

OBJS := $(patsubst ${SRC_DIR}/%.cpp,%.o,$(SRCS))
OBJS += parser.o scanner.o

DBG_OBJS := $(addprefix ${DEBUG_DIR}/,$(OBJS))
REL_OBJS := $(addprefix ${RELEASE_DIR}/,$(OBJS))

DEPS := $(addprefix ${DEPS_DIR}/, $(patsubst ${SRC_DIR}/%.cpp,%.d,$(SRCS)))

debug:   CPPFLAGS := -Og
release: CPPFLAGS := -O3

release debug: src/parser.h

.PHONY: all clean debug release sweep

all: src/parser.h release debug

sweep:
	rm -rf ${DEPS_DIR} 

clean:
	rm -f debug_bin release_bin ${GENERATED_FILES} src/parser.h
	rm -rf ${DEBUG_DIR}
	rm -rf ${RELEASE_DIR}

release: release_bin

release_bin: ${REL_OBJS}
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) $^ -o $@

debug: debug_bin 

debug_bin: $(DBG_OBJS)
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) $^ -o $@

parser.o: src/parser.cpp

scanner.o: src/scanner.cpp

src/parser.cpp: src/parser.ypp
	bison ${d} --verbose --graph -o src/parser.cpp src/parser.ypp

src/scanner.cpp: src/scanner.lex
	flex -Cfe -o src/scanner.cpp src/scanner.lex

src/parser.h:
	bison ${d} --defines=src/parser.h src/parser.ypp
	rm -f parser.tab.cpp

-include $(DEPS)

${RELEASE_DIR}: 
	mkdir -p ${RELEASE_DIR}

${DEPS_DIR}:
	mkdir -p ${DEPS_DIR}

${DEBUG_DIR}:
	mkdir -p ${DEBUG_DIR}

${DEBUG_DIR}/%.o: src/%.cpp | ${DEPS_DIR} ${DEBUG_DIR}
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@ -MF $(patsubst ${DEBUG_DIR}/%.o,${DEPS_DIR}/%.d,$@)

${RELEASE_DIR}/%.o: src/%.cpp | ${DEPS_DIR} ${RELEASE_DIR}
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@ -MF $(patsubst ${RELEASE_DIR}/%.o,${DEPS_DIR}/%.d,$@)

todo:
	grep -Rn TODO

test_debug: ${TEST_DIR}/*.cpp
	$(CXX) -DDEBUG_TOKENS $(CPPFLAGS) $^ -o $@ ${LD_FLAGS}
	./$@
	rm -f $@

test_release: ${TEST_DIR}/*.cpp
	$(CXX) -DDEBUG_TOKENS $(CPPFLAGS) $^ -o $@ ${LD_FLAGS}
	./$@
	rm -f $@

test_all: test_debug test_release

parser_graph: src/parser.ypp
	bison --defines=src/parser.h --verbose --graph -o src/parser.cpp src/parser.ypp
	dot -Tpng src/parser.dot -o parser.png

# Formats source files in a consistent way using astyle. To install astyle, go to
# https://astyle.sourceforge.net/. I put it in ../tools/ relative to this directory.
# Formatting rules are in .astylerc
fmt: ${SRCS} ${SRC_DIR}/*.h ${TEST_DIR}/*.cpp ${TEST_DIR}/*.h
	../tools/astyle --project=.astylerc $^
