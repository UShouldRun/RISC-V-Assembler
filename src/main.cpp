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

  parser::RISCVAST* ast = parser::parse(tokens, s_tokens);
  parser::check(ast);

  const int32_t error = (int32_t)ast->error;
  if (!error) {
    uint64_t s_insts = 0;
    const uint32_t* insts = mapper::map_inst2bin(ast, s_array);
    mapper::write(insts, s_insts);
    free(insts);
  }

  parser::ast_free(ast);
  lexer::riscv_tokens_free(tokens, s_tokens);

  return error;
}
