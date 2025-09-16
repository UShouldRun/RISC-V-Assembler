#include <iostream>
#include <fstream>

#include <cstdlib>
#include <cstdint>

#include "lexer.hpp"
#include "parser.hpp"
#include "mapper.hpp"

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
    mapper::RISCVEncoding encoding = {
      .s_insts = 0,
      .s_data  = 0,
      .insts   = nullptr,
      .data    = nullptr
    };

    encoding.data  = mapper::map_data2bin(ast, encoding.s_data);
    encoding.insts = mapper::map_inst2bin(ast, encoding.s_insts);
    error(
      FATAL,
      encoding.insts == nullptr,
      "main - mapper returned a nullptr array of instructions",
      "",
      __FILE__,
      __LINE__
    );

    mapper::write(filename, encoding);

    free(encoding.insts);
    if (encoding.data != nullptr)
      free(encoding.data);
  }

  parser::ast_free(ast);
  lexer::riscv_tokens_free(tokens, s_tokens);

  return error;
}
