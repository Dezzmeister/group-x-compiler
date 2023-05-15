#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include "asm.h"
#include "asm_utils.h"
#include "parsedecls.h"
#include "parseutils.h"
#include "parser.h"
#include "symtable.h"
#include "tac.h"

extern int yydebug;

#define MAX_STR_NUM 100

const char* str_literals[MAX_STR_NUM];

// Used to store string literals in read only text segment of assembly file.
void add_string(const char* str) {
  static int i = 0;
  str_literals[i++] = str;
}

char* get_option(char** begin, char** end, const std::string& option) {
  char** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return 0;
}

bool option_exists(char** begin, char** end, const std::string& option) {
  return std::find(begin, end, option) != end;
}

extern SymbolTable* symtable;

int main(int argc, char** argv) {
  x::setup_symtable();
  if (YYDEBUG) {
    yydebug = 1;
  } /* Enable tracing */

  // Keeps track of the number of command line arguments.
  int options = 1;
  bool graph = false;

  if (option_exists(argv, argv + argc, "--graph")) {
    ++options;
    graph = true;
  }

  ParseResult result = (argc == options + 1) ? x::parse_file(argv[options]) : x::parse_stdin();
  SymbolTable * symtable = result.parser_state->symtable;
  ProgramSource * top = result.parser_state->top;
  result.parser_state->errors.print(stderr);

  if (result.error) {
    perror("Error: ");
    return 1;
  }

  top->typecheck(symtable, result.parser_state->errors.sources[top]);
  result.parser_state->errors.print(stderr);

  std::fstream fs;
  fs.open("a.s", std::fstream::out);
  x::generate_assembly(result.parser_state->top, result.parser_state->symtable, fs);
  fs.close();

  if (graph) {
    std::ofstream dotfile;
    dotfile.open("prog.dot");
    x::tree_dotfile(dotfile, result.parser_state->top);
    dotfile.close();
  }
  return 0;
}
