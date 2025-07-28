#include <iostream>
#include <fstream>

#include <cstdlib>
#include <cstdint>

#include "lexer.hpp"
#include "parser.hpp"
// #include "mapper.hpp"

void print_help() {
  std::cout << "riscv <your_file.s>";
}

int32_t main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "[ERROR]: main - invalid arguments" << std::endl;
    print_help();
    exit(1);
  }

  const char* filename = argv[1];

  uint64_t s_tokens = 0;
  lexer::RISCVToken* tokens = lexer::lex(filename, s_tokens);

  /*
  for (uint64_t i = 0; i < s_tokens; i++)
    lexer::riscv_token_print(&tokens[i]);
   * */

  parser::RISCVAST* ast = parser::parse(tokens, s_tokens);
  parser::check(ast);
  // parser::ast_print(ast);

  parser::ast_free(ast);
  lexer::riscv_tokens_free(tokens, s_tokens);

  return 0;
}
