#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include "src/parsedecls.h"
#include "src/parser.h"
#include "src/symtable.h"

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
  ParserState parser_state;
  FILE * yyin = nullptr;
  yyscan_t scanner;
  yylex_init_extra(&parser_state, &scanner);

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

  if (argc == options + 1) {
    yyin = fopen(argv[options], "r");
    if (!yyin) {
      fprintf(stderr, "Cannot open file %s for reading.\n", argv[options]);
      exit(1);
    }
    close(0);
    if (dup(fileno(yyin))) {
      fprintf(stderr, "dup error.\n");
      exit(1);
    }
    yyset_in(yyin, scanner);
  }

  int val = yyparse(scanner, &parser_state);
  yylex_destroy(scanner);
  printf("status code: %d\n", val);
  printf("expr: ");
  parser_state.top->print();
  printf("\nkind: %s\n", x::kind_map[parser_state.top->get_kind()].c_str());

  if (yyin != nullptr) {
    fclose(yyin);
  }

  if (graph) {
    std::ofstream dotfile;
    dotfile.open("prog.dot");
    x::tree_dotfile(dotfile, parser_state.top);
    dotfile.close();
  }
  return val;
}
