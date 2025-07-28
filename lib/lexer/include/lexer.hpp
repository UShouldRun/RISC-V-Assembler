#ifndef __LEXER_H__
#define __LEXER_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "error.h"

namespace lexer {
  enum riscv_token_type {
    TOKEN_NONE,

    TOKEN_TEXT,
    TOKEN_DATA,

    TOKEN_BYTE,
    TOKEN_HALF,
    TOKEN_WORD,
    TOKEN_STRING,

    TOKEN_SYMBOL,

    TOKEN_LIT_STRING,
    TOKEN_LIT_NUMBER,

    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_PERIOD,
    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_REG_X0,
    TOKEN_REG_X1,
    TOKEN_REG_X2,
    TOKEN_REG_X3,
    TOKEN_REG_X4,
    TOKEN_REG_X5,
    TOKEN_REG_X6,
    TOKEN_REG_X7,
    TOKEN_REG_X8,
    TOKEN_REG_X9,
    TOKEN_REG_X10,
    TOKEN_REG_X11,
    TOKEN_REG_X12,
    TOKEN_REG_X13,
    TOKEN_REG_X14,
    TOKEN_REG_X15,
    TOKEN_REG_X16,
    TOKEN_REG_X17,
    TOKEN_REG_X18,
    TOKEN_REG_X19,
    TOKEN_REG_X20,
    TOKEN_REG_X21,
    TOKEN_REG_X22,
    TOKEN_REG_X23,
    TOKEN_REG_X24,
    TOKEN_REG_X25,
    TOKEN_REG_X26,
    TOKEN_REG_X27,
    TOKEN_REG_X28,
    TOKEN_REG_X29,
    TOKEN_REG_X30,
    TOKEN_REG_X31,

    TOKEN_INST_32IM_NOP,

    TOKEN_INST_32IM_MOVE_LI,
    TOKEN_INST_32IM_MOVE_LA,
    TOKEN_INST_32IM_MOVE_LUI,
    TOKEN_INST_32IM_MOVE_AUIPC,
    TOKEN_INST_32IM_MOVE_MV,

    TOKEN_INST_32IM_ALS_NEG,
    TOKEN_INST_32IM_ALS_ADD,
    TOKEN_INST_32IM_ALS_ADDI,
    TOKEN_INST_32IM_ALS_SUB,
    TOKEN_INST_32IM_ALS_NOT,
    TOKEN_INST_32IM_ALS_AND,
    TOKEN_INST_32IM_ALS_ANDI,
    TOKEN_INST_32IM_ALS_OR,
    TOKEN_INST_32IM_ALS_ORI,
    TOKEN_INST_32IM_ALS_XOR,
    TOKEN_INST_32IM_ALS_XORI,
    TOKEN_INST_32IM_ALS_SLL,
    TOKEN_INST_32IM_ALS_SLLI,
    TOKEN_INST_32IM_ALS_SRL,
    TOKEN_INST_32IM_ALS_SRLI,
    TOKEN_INST_32IM_ALS_SRA,
    TOKEN_INST_32IM_ALS_SRAI,

    TOKEN_INST_32IM_MD_MUL,
    TOKEN_INST_32IM_MD_MULH,
    TOKEN_INST_32IM_MD_MULSU,
    TOKEN_INST_32IM_MD_MULU,
    TOKEN_INST_32IM_MD_DIV,
    TOKEN_INST_32IM_MD_DIVU,
    TOKEN_INST_32IM_MD_REM,
    TOKEN_INST_32IM_MD_REMU,

    TOKEN_INST_32IM_LS_L,
    TOKEN_INST_32IM_LS_LB,
    TOKEN_INST_32IM_LS_LH,
    TOKEN_INST_32IM_LS_LW,
    TOKEN_INST_32IM_LS_LBU,
    TOKEN_INST_32IM_LS_LHU,
    TOKEN_INST_32IM_LS_S,
    TOKEN_INST_32IM_LS_SB,
    TOKEN_INST_32IM_LS_SH,
    TOKEN_INST_32IM_LS_SW,

    TOKEN_INST_32IM_CP_SLT,
    TOKEN_INST_32IM_CP_SLTI,
    TOKEN_INST_32IM_CP_SLTU,
    TOKEN_INST_32IM_CP_SLTIU,
    TOKEN_INST_32IM_CP_SEQZ,
    TOKEN_INST_32IM_CP_SNEZ,
    TOKEN_INST_32IM_CP_SLTZ,
    TOKEN_INST_32IM_CP_SGTZ,

    TOKEN_INST_32IM_FC_BEQ,
    TOKEN_INST_32IM_FC_BNE,
    TOKEN_INST_32IM_FC_BGT,
    TOKEN_INST_32IM_FC_BGE,
    TOKEN_INST_32IM_FC_BLE,
    TOKEN_INST_32IM_FC_BLT,
    TOKEN_INST_32IM_FC_BGTU,
    TOKEN_INST_32IM_FC_BGEU,
    TOKEN_INST_32IM_FC_BLTU,
    TOKEN_INST_32IM_FC_BLEU,
    TOKEN_INST_32IM_FC_BEQZ,
    TOKEN_INST_32IM_FC_BNEZ,
    TOKEN_INST_32IM_FC_BLEZ,
    TOKEN_INST_32IM_FC_BGEZ,
    TOKEN_INST_32IM_FC_BLTZ,
    TOKEN_INST_32IM_FC_BGTZ,
    TOKEN_INST_32IM_FC_J,
    TOKEN_INST_32IM_FC_JAL,
    TOKEN_INST_32IM_FC_JR,
    TOKEN_INST_32IM_FC_JALR,
    TOKEN_INST_32IM_FC_CALL,
    TOKEN_INST_32IM_FC_RET,

    TOKEN_INST_32IM_OS_ECALL,
    TOKEN_INST_32IM_OS_EBREAK,
    TOKEN_INST_32IM_OS_SRET,
    
    TOKEN_INST_32IM_LNS_ADD,
    TOKEN_INST_32IM_LNS_SUB,
    TOKEN_INST_32IM_LNS_MUL,
    TOKEN_INST_32IM_LNS_DIV,
    TOKEN_INST_32IM_LNS_SQT,
    
    TOKEN_INST_32IM_MAX
  };

  typedef enum riscv_token_type RISCVTokenType;
  typedef struct riscv_token    RISCVToken;

  RISCVToken* lex                         (const char*, uint64_t&);

  void        riscv_token_print           (const RISCVToken*);
  void        riscv_tokens_free           (RISCVToken*, const uint64_t);

  const char* riscv_token_get_type_string (const RISCVTokenType);

  bool        riscv_token_is_reg          (const RISCVTokenType);
  bool        riscv_token_is_inst         (const RISCVTokenType);
  bool        riscv_token_is_param        (const RISCVTokenType);
  bool        riscv_token_is_lit          (const RISCVTokenType);
  bool        riscv_token_is_symbol_type  (const RISCVTokenType);

  struct riscv_token {
    RISCVTokenType type;
    union {
      char*    string;
      int32_t number;
    } lit;
    uint32_t line, start, end;
    char* filename;
  };
}

#endif // !__LEXER_H__
