#ifndef __PARSER_H_
#define __PARSER_H_

#include "error.hpp"
#include "lexer.hpp"

namespace parser {
  typedef struct riscv_ast       RISCVAST;
  typedef struct riscv_astn_text RISCVASTN_Text;
  typedef struct riscv_astn_data RISCVASTN_Data;

  RISCVAST*   parser         (const lexer::RISCVToken*, const uint64_t);
  void        rewrite_pseudo (RISCVAST*);

  struct riscv_astn_text {
    lexer::RISCVToken
      *inst, *f1, *f2, *f3, *f4;
  };

  struct riscv_astn_data {
    lexer::RISCVToken
      *label, *type, *data;
  };

  struct riscv_ast {
    bool error;
    uint64_t s_data, s_text;
    struct riscv_astn_data* data;
    struct riscv_astn_text  text[];
  };
}

#endif // !__PARSER_H_
