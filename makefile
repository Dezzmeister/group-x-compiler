COMMON_FLAGS = -Wall -Werror -std=gnu++17 -march=native -fsanitize=unreachable
SRCS := $(wildcard src/*.cpp)

SRC_DIR  := src
DEP_DIR  := deps
TEST_DIR := tests

GENERATED_FILES := ${addprefix ${SRC_DIR}/,scanner.cpp parser.cpp}

OBJS := $(patsubst ${SRC_DIR}/%.cpp,%.o,$(SRCS))
OBJS += parser.o scanner.o

DEPENDS := $(addprefix $(DEP_DIR)/, $(patsubst ${SRC_DIR}/%.cpp,%.d,$(SRCS)))

debug: CPPFLAGS := -Og
release: CPPFLAGS := -O3

.PHONY: all clean debug release sweep

all: src/parser.h debug release 

sweep:
	rm -rf deps 

clean:
	rm -f debug_bin release_bin *.o ${GENERATED_FILES}

release: src/parser.h release_bin

release_bin: $(OBJS)
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) $^ -o $@

debug: src/parser.h debug_bin 

debug_bin: $(OBJS)
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) $^ -o $@

parser.o: src/parser.cpp

scanner.o: src/scanner.cpp

src/parser.cpp: src/parser.ypp
	bison --verbose --graph -o src/parser.cpp src/parser.ypp

src/scanner.cpp: src/scanner.lex
	flex -Cfe -o src/scanner.cpp src/scanner.lex

src/parser.h:
	bison --defines=src/parser.h src/parser.ypp

-include $(DEPENDS)

$(DEP_DIR):
	mkdir -p $(DEP_DIR)

%.o: src/%.cpp src/parser.h | $(DEP_DIR)
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@ -MF $(patsubst %.o,${DEP_DIR}/%.d,$@)

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
fmt: ${SRC_FILES} ${SRC_DIR}/*.h ${TEST_DIR}/*.cpp ${TEST_DIR}/*.h
	../tools/astyle --project=.astylerc $^