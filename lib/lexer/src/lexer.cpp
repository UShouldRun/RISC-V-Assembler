#include "lexer_private.hpp"

namespace lexer {
  RISCVToken* lex(const char* filename, uint64_t& s_tokens) {
    std::ifstream file(filename);
    error(FATAL, !file.is_open(), "lexer - could not open input file ", filename, __FILE__, __LINE__);
    log("lexer - opened input file ", filename, __FILE__, __LINE__);

    uint64_t max_s_tokens = 1 << 8;
    RISCVToken* tokens = (RISCVToken*)malloc(max_s_tokens * sizeof(struct riscv_token));
    error(FATAL, tokens == nullptr, "lexer - tokens array is a nullptr", "", __FILE__, __LINE__);

    std::string line;  
    for (uint32_t i = 1; std::getline(file, line); i++) {
      char* str = (char*)line.c_str();
      _lexer_scan_line(&tokens, s_tokens, max_s_tokens, str, filename, i);
      log("lexer - scanned line ", i, __FILE__, __LINE__);
    }

    if (s_tokens != max_s_tokens) {
      tokens = (lexer::RISCVToken*)realloc(tokens, s_tokens * sizeof(struct riscv_token));
      error(FATAL, tokens == nullptr, "lexer - tokens array is a nullptr after final reallocation", "", __FILE__, __LINE__);
    }

    file.close();
    return tokens;
  }

  void riscv_token_print(const RISCVToken* token) {
    error(FATAL, token == nullptr, "lexer - token is nullptr", "", __FILE__, __LINE__);

    printf("Token {\n");
    printf("  Type: %s,\n", riscv_token_get_type_string(token->type));
    printf(
      "  Location: (%s, %u, %u-%u)%c\n",
      token->filename ? token->filename : "N/A",
      token->line, token->start, token->end,
      token->type == TOKEN_LIT_STRING || token->type == TOKEN_LIT_NUMBER ? ',' : '\0'
    );

    switch (token->type) {
      case TOKEN_LIT_STRING: case TOKEN_SYMBOL: {
        printf("  Literal (String): \"%s\"\n", token->lit.string ? token->lit.string : "(NULL)");
        break;
      }
      case TOKEN_LIT_NUMBER: {
        printf("  Literal (Number): %d\n", token->lit.number);
        break;
      }
      default: {
        break;
      }
    }
    printf("}\n");
  } 

  void riscv_tokens_free(RISCVToken* tokens, const uint64_t s_tokens) {
   if (tokens == NULL)
      return;
    for (uint64_t i = 0; i < s_tokens; i++)
      if (tokens[i].type == TOKEN_SYMBOL || tokens[i].type == TOKEN_LIT_STRING)
        free(tokens[i].lit.string);
    free(tokens);
  }

  const char* riscv_token_get_type_string(const RISCVTokenType type) {
    switch (type) {
      case lexer::TOKEN_NONE:                 return "TOKEN_NONE";
      case lexer::TOKEN_TEXT:                 return "TOKEN_TEXT";
      case lexer::TOKEN_DATA:                 return "TOKEN_DATA";
      case lexer::TOKEN_BYTE:                 return "TOKEN_BYTE";
      case lexer::TOKEN_HALF:                 return "TOKEN_HALF";
      case lexer::TOKEN_WORD:                 return "TOKEN_WORD";
      case lexer::TOKEN_STRING:               return "TOKEN_STRING";
      case lexer::TOKEN_LIT_STRING:           return "TOKEN_LIT_STRING";
      case lexer::TOKEN_LIT_NUMBER:           return "TOKEN_LIT_NUMBER";
      case lexer::TOKEN_SYMBOL:               return "TOKEN_SYMBOL";
      case lexer::TOKEN_COLON:                 return "TOKEN_COLON";
      case lexer::TOKEN_COMMA:                 return "TOKEN_COMMA";
      case lexer::TOKEN_LPAREN:                return "TOKEN_LPAREN";
      case lexer::TOKEN_RPAREN:                return "TOKEN_RPAREN";

      case lexer::TOKEN_REG_X0:               return "TOKEN_REG_X0 (zero/x0)";
      case lexer::TOKEN_REG_X1:               return "TOKEN_REG_X1 (ra/x1)";
      case lexer::TOKEN_REG_X2:               return "TOKEN_REG_X2 (sp/x2)";
      case lexer::TOKEN_REG_X3:               return "TOKEN_REG_X3 (gp/x3)";
      case lexer::TOKEN_REG_X4:               return "TOKEN_REG_X4 (tp/x4)";
      case lexer::TOKEN_REG_X5:               return "TOKEN_REG_X5 (t0/x5)";
      case lexer::TOKEN_REG_X6:               return "TOKEN_REG_X6 (t1/x6)";
      case lexer::TOKEN_REG_X7:               return "TOKEN_REG_X7 (t2/x7)";
      case lexer::TOKEN_REG_X8:               return "TOKEN_REG_X8 (s0/fp/x8)";
      case lexer::TOKEN_REG_X9:               return "TOKEN_REG_X9 (s1/x9)";
      case lexer::TOKEN_REG_X10:              return "TOKEN_REG_X10 (a0/x10)";
      case lexer::TOKEN_REG_X11:              return "TOKEN_REG_X11 (a1/x11)";
      case lexer::TOKEN_REG_X12:              return "TOKEN_REG_X12 (a2/x12)";
      case lexer::TOKEN_REG_X13:              return "TOKEN_REG_X13 (a3/x13)";
      case lexer::TOKEN_REG_X14:              return "TOKEN_REG_X14 (a4/x14)";
      case lexer::TOKEN_REG_X15:              return "TOKEN_REG_X15 (a5/x15)";
      case lexer::TOKEN_REG_X16:              return "TOKEN_REG_X16 (a6/x16)";
      case lexer::TOKEN_REG_X17:              return "TOKEN_REG_X17 (a7/x17)";
      case lexer::TOKEN_REG_X18:              return "TOKEN_REG_X18 (s2/x18)";
      case lexer::TOKEN_REG_X19:              return "TOKEN_REG_X19 (s3/x19)";
      case lexer::TOKEN_REG_X20:              return "TOKEN_REG_X20 (s4/x20)";
      case lexer::TOKEN_REG_X21:              return "TOKEN_REG_X21 (s5/x21)";
      case lexer::TOKEN_REG_X22:              return "TOKEN_REG_X22 (s6/x22)";
      case lexer::TOKEN_REG_X23:              return "TOKEN_REG_X23 (s7/x23)";
      case lexer::TOKEN_REG_X24:              return "TOKEN_REG_X24 (s8/x24)";
      case lexer::TOKEN_REG_X25:              return "TOKEN_REG_X25 (s9/x25)";
      case lexer::TOKEN_REG_X26:              return "TOKEN_REG_X26 (s10/x26)";
      case lexer::TOKEN_REG_X27:              return "TOKEN_REG_X27 (s11/x27)";
      case lexer::TOKEN_REG_X28:              return "TOKEN_REG_X28 (t3/x28)";
      case lexer::TOKEN_REG_X29:              return "TOKEN_REG_X29 (t4/x29)";
      case lexer::TOKEN_REG_X30:              return "TOKEN_REG_X30 (t5/x30)";
      case lexer::TOKEN_REG_X31:              return "TOKEN_REG_X31 (t6/x31)";

      case lexer::TOKEN_INST_32IM_NOP:        return "TOKEN_INST_32IM_NOP";

      case lexer::TOKEN_INST_32IM_MOVE_LI:    return "TOKEN_INST_32IM_MOVE_LI";
      case lexer::TOKEN_INST_32IM_MOVE_LA:    return "TOKEN_INST_32IM_MOVE_LA";
      case lexer::TOKEN_INST_32IM_MOVE_LUI:   return "TOKEN_INST_32IM_MOVE_LUI";
      case lexer::TOKEN_INST_32IM_MOVE_AUIPC: return "TOKEN_INST_32IM_MOVE_AUIPC";
      case lexer::TOKEN_INST_32IM_MOVE_MV:    return "TOKEN_INST_32IM_MOVE_MV";

      case lexer::TOKEN_INST_32IM_ALS_NEG:    return "TOKEN_INST_32IM_ALS_NEG";
      case lexer::TOKEN_INST_32IM_ALS_ADD:    return "TOKEN_INST_32IM_ALS_ADD";
      case lexer::TOKEN_INST_32IM_ALS_ADDI:   return "TOKEN_INST_32IM_ALS_ADDI";
      case lexer::TOKEN_INST_32IM_ALS_SUB:    return "TOKEN_INST_32IM_ALS_SUB";
      case lexer::TOKEN_INST_32IM_ALS_NOT:    return "TOKEN_INST_32IM_ALS_NOT";
      case lexer::TOKEN_INST_32IM_ALS_AND:    return "TOKEN_INST_32IM_ALS_AND";
      case lexer::TOKEN_INST_32IM_ALS_ANDI:   return "TOKEN_INST_32IM_ALS_ANDI";
      case lexer::TOKEN_INST_32IM_ALS_OR:     return "TOKEN_INST_32IM_ALS_OR";
      case lexer::TOKEN_INST_32IM_ALS_ORI:    return "TOKEN_INST_32IM_ALS_ORI";
      case lexer::TOKEN_INST_32IM_ALS_XOR:    return "TOKEN_INST_32IM_ALS_XOR";
      case lexer::TOKEN_INST_32IM_ALS_XORI:   return "TOKEN_INST_32IM_ALS_XORI";
      case lexer::TOKEN_INST_32IM_ALS_SLL:    return "TOKEN_INST_32IM_ALS_SLL";
      case lexer::TOKEN_INST_32IM_ALS_SLLI:   return "TOKEN_INST_32IM_ALS_SLLI";
      case lexer::TOKEN_INST_32IM_ALS_SRL:    return "TOKEN_INST_32IM_ALS_SRL";
      case lexer::TOKEN_INST_32IM_ALS_SRLI:   return "TOKEN_INST_32IM_ALS_SRLI";
      case lexer::TOKEN_INST_32IM_ALS_SRA:    return "TOKEN_INST_32IM_ALS_SRA";
      case lexer::TOKEN_INST_32IM_ALS_SRAI:   return "TOKEN_INST_32IM_ALS_SRAI";

      case lexer::TOKEN_INST_32IM_MD_MUL:     return "TOKEN_INST_32IM_MD_MUL";
      case lexer::TOKEN_INST_32IM_MD_MULH:    return "TOKEN_INST_32IM_MD_MULH";
      case lexer::TOKEN_INST_32IM_MD_MULSU:   return "TOKEN_INST_32IM_MD_MULSU";
      case lexer::TOKEN_INST_32IM_MD_MULU:    return "TOKEN_INST_32IM_MD_MULU";
      case lexer::TOKEN_INST_32IM_MD_DIV:     return "TOKEN_INST_32IM_MD_DIV";
      case lexer::TOKEN_INST_32IM_MD_DIVU:    return "TOKEN_INST_32IM_MD_DIVU";
      case lexer::TOKEN_INST_32IM_MD_REM:     return "TOKEN_INST_32IM_MD_REM";
      case lexer::TOKEN_INST_32IM_MD_REMU:    return "TOKEN_INST_32IM_MD_REMU";

      case lexer::TOKEN_INST_32IM_LS_LB:      return "TOKEN_INST_32IM_LS_LB";
      case lexer::TOKEN_INST_32IM_LS_LH:      return "TOKEN_INST_32IM_LS_LH";
      case lexer::TOKEN_INST_32IM_LS_LW:      return "TOKEN_INST_32IM_LS_LW";
      case lexer::TOKEN_INST_32IM_LS_LBU:     return "TOKEN_INST_32IM_LS_LBU";
      case lexer::TOKEN_INST_32IM_LS_LHU:     return "TOKEN_INST_32IM_LS_LHU";
      case lexer::TOKEN_INST_32IM_LS_SB:      return "TOKEN_INST_32IM_LS_SB";
      case lexer::TOKEN_INST_32IM_LS_SH:      return "TOKEN_INST_32IM_LS_SH";
      case lexer::TOKEN_INST_32IM_LS_SW:      return "TOKEN_INST_32IM_LS_SW";

      case lexer::TOKEN_INST_32IM_CP_SLT:     return "TOKEN_INST_32IM_CP_SLT";
      case lexer::TOKEN_INST_32IM_CP_SLTI:    return "TOKEN_INST_32IM_CP_SLTI";
      case lexer::TOKEN_INST_32IM_CP_SLTU:    return "TOKEN_INST_32IM_CP_SLTU";
      case lexer::TOKEN_INST_32IM_CP_SLTIU:   return "TOKEN_INST_32IM_CP_SLTIU";
      case lexer::TOKEN_INST_32IM_CP_SEQZ:    return "TOKEN_INST_32IM_CP_SEQZ";
      case lexer::TOKEN_INST_32IM_CP_SNEZ:    return "TOKEN_INST_32IM_CP_SNEZ";
      case lexer::TOKEN_INST_32IM_CP_SLTZ:    return "TOKEN_INST_32IM_CP_SLTZ";
      case lexer::TOKEN_INST_32IM_CP_SGTZ:    return "TOKEN_INST_32IM_CP_SGTZ";

      case lexer::TOKEN_INST_32IM_FC_BEQ:     return "TOKEN_INST_32IM_FC_BEQ";
      case lexer::TOKEN_INST_32IM_FC_BNE:     return "TOKEN_INST_32IM_FC_BNE";
      case lexer::TOKEN_INST_32IM_FC_BGT:     return "TOKEN_INST_32IM_FC_BGT";
      case lexer::TOKEN_INST_32IM_FC_BGE:     return "TOKEN_INST_32IM_FC_BGE";
      case lexer::TOKEN_INST_32IM_FC_BLE:     return "TOKEN_INST_32IM_FC_BLE";
      case lexer::TOKEN_INST_32IM_FC_BLT:     return "TOKEN_INST_32IM_FC_BLT";
      case lexer::TOKEN_INST_32IM_FC_BGTU:    return "TOKEN_INST_32IM_FC_BGTU";
      case lexer::TOKEN_INST_32IM_FC_BGEU:    return "TOKEN_INST_32IM_FC_BGEU";
      case lexer::TOKEN_INST_32IM_FC_BLTU:    return "TOKEN_INST_32IM_FC_BLTU";
      case lexer::TOKEN_INST_32IM_FC_BLEU:    return "TOKEN_INST_32IM_FC_BLEU";
      case lexer::TOKEN_INST_32IM_FC_BEQZ:    return "TOKEN_INST_32IM_FC_BEQZ";
      case lexer::TOKEN_INST_32IM_FC_BNEZ:    return "TOKEN_INST_32IM_FC_BNEZ";
      case lexer::TOKEN_INST_32IM_FC_BLEZ:    return "TOKEN_INST_32IM_FC_BLEZ";
      case lexer::TOKEN_INST_32IM_FC_BGEZ:    return "TOKEN_INST_32IM_FC_BGEZ";
      case lexer::TOKEN_INST_32IM_FC_BLTZ:    return "TOKEN_INST_32IM_FC_BLTZ";
      case lexer::TOKEN_INST_32IM_FC_BGTZ:    return "TOKEN_INST_32IM_FC_BGTZ";
      case lexer::TOKEN_INST_32IM_FC_J:       return "TOKEN_INST_32IM_FC_J";
      case lexer::TOKEN_INST_32IM_FC_JAL:     return "TOKEN_INST_32IM_FC_JAL";
      case lexer::TOKEN_INST_32IM_FC_JR:      return "TOKEN_INST_32IM_FC_JR";
      case lexer::TOKEN_INST_32IM_FC_JALR:    return "TOKEN_INST_32IM_FC_JALR";
      case lexer::TOKEN_INST_32IM_FC_CALL:    return "TOKEN_INST_32IM_FC_CALL";
      case lexer::TOKEN_INST_32IM_FC_RET:     return "TOKEN_INST_32IM_FC_RET";

      case lexer::TOKEN_INST_32IM_OS_ECALL:   return "TOKEN_INST_32IM_OS_ECALL";
      case lexer::TOKEN_INST_32IM_OS_EBREAK:  return "TOKEN_INST_32IM_OS_EBREAK";
      case lexer::TOKEN_INST_32IM_OS_SRET:    return "TOKEN_INST_32IM_OS_SRET";
      
      case lexer::TOKEN_INST_32IM_LNS_ADD:    return "TOKEN_INST_32IM_LNS_ADD";
      case lexer::TOKEN_INST_32IM_LNS_SUB:    return "TOKEN_INST_32IM_LNS_SUB";
      case lexer::TOKEN_INST_32IM_LNS_MUL:    return "TOKEN_INST_32IM_LNS_MUL";
      case lexer::TOKEN_INST_32IM_LNS_DIV:    return "TOKEN_INST_32IM_LNS_DIV";
      case lexer::TOKEN_INST_32IM_LNS_SQT:    return "TOKEN_INST_32IM_LNS_SQT";

      default:                         return "UNKNOWN_TOKEN_TYPE";
    }
  }

  inline uint64_t riscv_token_get_type_size(const RISCVTokenType type) {
    error(
      FATAL,
      (type >= lexer::TOKEN_BYTE && type <= lexer::TOKEN_WORD) || type == lexer::TOKEN_STRING,
      "lexer - type is outside function domain: ",
      __FUNCTION__,
      __FILE__,
      __LINE__
    );
    return type == TOKEN_STRING ? 1 : (1 << (type - lexer::TOKEN_BYTE));
  }

  inline uint8_t riscv_token_get_reg(const RISCVTokenType type) {
    error(
      FATAL,
      !riscv_token_is_reg(type),
      "lexer - type is outside of function domain: ",
      __FUNCTION__,
      __FILE__,
      __LINE__
    );
    return (uint8_t)(type - TOKEN_REG_X0);
  }

  inline bool riscv_token_is_reg(const RISCVTokenType type) {
    return type >= TOKEN_REG_X0 && type <= TOKEN_REG_X31;
  }

  inline bool riscv_token_is_inst(const RISCVTokenType type) {
    return type >= TOKEN_INST_32IM_NOP && type < TOKEN_INST_32IM_MAX;
  }

  inline bool riscv_token_is_param(const RISCVTokenType type) {
    return riscv_token_is_reg(type) || type == TOKEN_LIT_NUMBER || type == TOKEN_SYMBOL;
  }

  inline bool riscv_token_is_lit(const RISCVTokenType type) {
    return type == TOKEN_LIT_NUMBER || type == TOKEN_LIT_STRING;
  }

  inline bool riscv_token_is_symbol_type(const RISCVTokenType type) {
    return type == TOKEN_BYTE || type == TOKEN_HALF || type == TOKEN_WORD || type == TOKEN_STRING;
  }
}

void _lexer_scan_line(
  lexer::RISCVToken** tokens, uint64_t& s_tokens, uint64_t& max_s_tokens, char* str,
  const char* filename, const uint32_t line
) {
  error(FATAL, str == nullptr, "lexer - scanned str is somehow a NULL pointer", "", __FILE__, __LINE__);
  log("lexer - scanning line: ", str, __FILE__, __LINE__);

  const uint8_t max_s_token = 1 << 7;
  uint32_t s_chs = 0;
  for (uint32_t i = 1; *str != CHAR_END; i += s_chs, str += s_chs) {
    s_chs = _lexer_skip_space(str) + _lexer_skip_comments(str);
    log("lexer - skipped space and comments ", s_chs, filename, line);
    if (s_chs > 0)
      continue;

    *tokens = _lexer_tokens_realloc(*tokens, s_tokens, max_s_tokens);
    
    char token[max_s_token];
    s_chs = _lexer_scan_next(token, max_s_token, str, filename, line);
    log("lexer - scanned next token ", token, filename, line);

    lexer::RISCVTokenType type = lexer::TOKEN_NONE;
    if (s_chs == 1 && token[0] == CHAR_COMMA) {
      type = lexer::TOKEN_COMMA;
    } else if (s_chs == 1 && token[0] == CHAR_COLON) {
      type = lexer::TOKEN_COLON;
    } else if (s_chs == 1 && token[0] == CHAR_LPAREN) {
      type = lexer::TOKEN_LPAREN;
    } else if (s_chs == 1 && token[0] == CHAR_RPAREN) {
      type = lexer::TOKEN_RPAREN;
    } else if (strcmp(token, KEYWORD_TEXT) == 0) {
      type = lexer::TOKEN_TEXT;
    } else if (strcmp(token, KEYWORD_DATA) == 0) {
      type = lexer::TOKEN_DATA;
    } else if (strcmp(token, KEYWORD_BYTE) == 0) {
      type = lexer::TOKEN_BYTE;
    } else if (strcmp(token, KEYWORD_HALF) == 0) {
      type = lexer::TOKEN_HALF;
    } else if (strcmp(token, KEYWORD_WORD) == 0) {
      type = lexer::TOKEN_WORD;
    } else if (strcmp(token, KEYWORD_STRING) == 0) {
      type = lexer::TOKEN_STRING;
    } else if (strcmp(token, KEYWORD_ZERO) == 0 || strcmp(token, KEYWORD_X0) == 0) {
      type = lexer::TOKEN_REG_X0;
    } else if (strcmp(token, KEYWORD_RA) == 0 || strcmp(token, KEYWORD_X1) == 0) {
      type = lexer::TOKEN_REG_X1;
    } else if (strcmp(token, KEYWORD_SP) == 0 || strcmp(token, KEYWORD_X2) == 0) {
      type = lexer::TOKEN_REG_X2;
    } else if (strcmp(token, KEYWORD_GP) == 0 || strcmp(token, KEYWORD_X3) == 0) {
      type = lexer::TOKEN_REG_X3;
    } else if (strcmp(token, KEYWORD_TP) == 0 || strcmp(token, KEYWORD_X4) == 0) {
      type = lexer::TOKEN_REG_X4;
    } else if (strcmp(token, KEYWORD_T0) == 0 || strcmp(token, KEYWORD_X5) == 0) {
      type = lexer::TOKEN_REG_X5;
    } else if (strcmp(token, KEYWORD_T1) == 0 || strcmp(token, KEYWORD_X6) == 0) {
      type = lexer::TOKEN_REG_X6;
    } else if (strcmp(token, KEYWORD_T2) == 0 || strcmp(token, KEYWORD_X7) == 0) {
      type = lexer::TOKEN_REG_X7;
    } else if (strcmp(token, KEYWORD_S0) == 0 || strcmp(token, KEYWORD_X8) == 0) {
      type = lexer::TOKEN_REG_X8;
    } else if (strcmp(token, KEYWORD_S1) == 0 || strcmp(token, KEYWORD_X9) == 0) {
      type = lexer::TOKEN_REG_X9;
    } else if (strcmp(token, KEYWORD_A0) == 0 || strcmp(token, KEYWORD_X10) == 0) {
      type = lexer::TOKEN_REG_X10;
    } else if (strcmp(token, KEYWORD_A1) == 0 || strcmp(token, KEYWORD_X11) == 0) {
      type = lexer::TOKEN_REG_X11;
    } else if (strcmp(token, KEYWORD_A2) == 0 || strcmp(token, KEYWORD_X12) == 0) {
      type = lexer::TOKEN_REG_X12;
    } else if (strcmp(token, KEYWORD_A3) == 0 || strcmp(token, KEYWORD_X13) == 0) {
      type = lexer::TOKEN_REG_X13;
    } else if (strcmp(token, KEYWORD_A4) == 0 || strcmp(token, KEYWORD_X14) == 0) {
      type = lexer::TOKEN_REG_X14;
    } else if (strcmp(token, KEYWORD_A5) == 0 || strcmp(token, KEYWORD_X15) == 0) {
      type = lexer::TOKEN_REG_X15;
    } else if (strcmp(token, KEYWORD_A6) == 0 || strcmp(token, KEYWORD_X16) == 0) {
      type = lexer::TOKEN_REG_X16;
    } else if (strcmp(token, KEYWORD_A7) == 0 || strcmp(token, KEYWORD_X17) == 0) {
      type = lexer::TOKEN_REG_X17;
    } else if (strcmp(token, KEYWORD_S2) == 0 || strcmp(token, KEYWORD_X18) == 0) {
      type = lexer::TOKEN_REG_X18;
    } else if (strcmp(token, KEYWORD_S3) == 0 || strcmp(token, KEYWORD_X19) == 0) {
      type = lexer::TOKEN_REG_X19;
    } else if (strcmp(token, KEYWORD_S4) == 0 || strcmp(token, KEYWORD_X20) == 0) {
      type = lexer::TOKEN_REG_X20;
    } else if (strcmp(token, KEYWORD_S5) == 0 || strcmp(token, KEYWORD_X21) == 0) {
      type = lexer::TOKEN_REG_X21;
    } else if (strcmp(token, KEYWORD_S6) == 0 || strcmp(token, KEYWORD_X22) == 0) {
      type = lexer::TOKEN_REG_X22;
    } else if (strcmp(token, KEYWORD_S7) == 0 || strcmp(token, KEYWORD_X23) == 0) {
      type = lexer::TOKEN_REG_X23;
    } else if (strcmp(token, KEYWORD_S8) == 0 || strcmp(token, KEYWORD_X24) == 0) {
      type = lexer::TOKEN_REG_X24;
    } else if (strcmp(token, KEYWORD_S9) == 0 || strcmp(token, KEYWORD_X25) == 0) {
      type = lexer::TOKEN_REG_X25;
    } else if (strcmp(token, KEYWORD_S10) == 0 || strcmp(token, KEYWORD_X26) == 0) {
      type = lexer::TOKEN_REG_X26;
    } else if (strcmp(token, KEYWORD_S11) == 0 || strcmp(token, KEYWORD_X27) == 0) {
      type = lexer::TOKEN_REG_X27;
    } else if (strcmp(token, KEYWORD_T3) == 0 || strcmp(token, KEYWORD_X28) == 0) {
      type = lexer::TOKEN_REG_X28;
    } else if (strcmp(token, KEYWORD_T4) == 0 || strcmp(token, KEYWORD_X29) == 0) {
      type = lexer::TOKEN_REG_X29;
    } else if (strcmp(token, KEYWORD_T5) == 0 || strcmp(token, KEYWORD_X30) == 0) {
      type = lexer::TOKEN_REG_X30;
    } else if (strcmp(token, KEYWORD_T6) == 0 || strcmp(token, KEYWORD_X31) == 0) {
      type = lexer::TOKEN_REG_X31;
    } else if (strcmp(token, KEYWORD_NOP) == 0) {
      type = lexer::TOKEN_INST_32IM_NOP;
    } else if (strcmp(token, KEYWORD_LI) == 0) {
      type = lexer::TOKEN_INST_32IM_MOVE_LI;
    } else if (strcmp(token, KEYWORD_LA) == 0) {
      type = lexer::TOKEN_INST_32IM_MOVE_LA;
    } else if (strcmp(token, KEYWORD_LUI) == 0) {
      type = lexer::TOKEN_INST_32IM_MOVE_LUI;
    } else if (strcmp(token, KEYWORD_AUIPC) == 0) {
      type = lexer::TOKEN_INST_32IM_MOVE_AUIPC;
    } else if (strcmp(token, KEYWORD_MV) == 0) {
      type = lexer::TOKEN_INST_32IM_MOVE_MV;
    } else if (strcmp(token, KEYWORD_NEG) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_NEG;
    } else if (strcmp(token, KEYWORD_ADD) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_ADD;
    } else if (strcmp(token, KEYWORD_ADDI) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_ADDI;
    } else if (strcmp(token, KEYWORD_SUB) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_SUB;
    } else if (strcmp(token, KEYWORD_NOT) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_NOT;
    } else if (strcmp(token, KEYWORD_AND) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_AND;
    } else if (strcmp(token, KEYWORD_ANDI) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_ANDI;
    } else if (strcmp(token, KEYWORD_OR) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_OR;
    } else if (strcmp(token, KEYWORD_ORI) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_ORI;
    } else if (strcmp(token, KEYWORD_XOR) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_XOR;
    } else if (strcmp(token, KEYWORD_XORI) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_XORI;
    } else if (strcmp(token, KEYWORD_SLL) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_SLL;
    } else if (strcmp(token, KEYWORD_SLLI) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_SLLI;
    } else if (strcmp(token, KEYWORD_SRL) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_SRL;
    } else if (strcmp(token, KEYWORD_SRLI) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_SRLI;
    } else if (strcmp(token, KEYWORD_SRA) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_SRA;
    } else if (strcmp(token, KEYWORD_SRAI) == 0) {
      type = lexer::TOKEN_INST_32IM_ALS_SRAI;
    } else if (strcmp(token, KEYWORD_MUL) == 0) {
      type = lexer::TOKEN_INST_32IM_MD_MUL;
    } else if (strcmp(token, KEYWORD_MULH) == 0) {
      type = lexer::TOKEN_INST_32IM_MD_MULH;
    } else if (strcmp(token, KEYWORD_MULSU) == 0) {
      type = lexer::TOKEN_INST_32IM_MD_MULSU;
    } else if (strcmp(token, KEYWORD_MULU) == 0) {
      type = lexer::TOKEN_INST_32IM_MD_MULU;
    } else if (strcmp(token, KEYWORD_DIV) == 0) {
      type = lexer::TOKEN_INST_32IM_MD_DIV;
    } else if (strcmp(token, KEYWORD_DIVU) == 0) {
      type = lexer::TOKEN_INST_32IM_MD_DIVU;
    } else if (strcmp(token, KEYWORD_REM) == 0) {
      type = lexer::TOKEN_INST_32IM_MD_REM;
    } else if (strcmp(token, KEYWORD_REMU) == 0) {
      type = lexer::TOKEN_INST_32IM_MD_REMU;
    } else if (strcmp(token, KEYWORD_LB) == 0) {
      type = lexer::TOKEN_INST_32IM_LS_LB;
    } else if (strcmp(token, KEYWORD_LH) == 0) {
      type = lexer::TOKEN_INST_32IM_LS_LH;
    } else if (strcmp(token, KEYWORD_LW) == 0) {
      type = lexer::TOKEN_INST_32IM_LS_LW;
    } else if (strcmp(token, KEYWORD_LBU) == 0) {
      type = lexer::TOKEN_INST_32IM_LS_LBU;
    } else if (strcmp(token, KEYWORD_LHU) == 0) {
      type = lexer::TOKEN_INST_32IM_LS_LHU;
    } else if (strcmp(token, KEYWORD_SB) == 0) {
      type = lexer::TOKEN_INST_32IM_LS_SB;
    } else if (strcmp(token, KEYWORD_SH) == 0) {
      type = lexer::TOKEN_INST_32IM_LS_SH;
    } else if (strcmp(token, KEYWORD_SW) == 0) {
      type = lexer::TOKEN_INST_32IM_LS_SW;
    } else if (strcmp(token, KEYWORD_SLT) == 0) {
      type = lexer::TOKEN_INST_32IM_CP_SLT;
    } else if (strcmp(token, KEYWORD_SLTI) == 0) {
      type = lexer::TOKEN_INST_32IM_CP_SLTI;
    } else if (strcmp(token, KEYWORD_SLTU) == 0) {
      type = lexer::TOKEN_INST_32IM_CP_SLTU;
    } else if (strcmp(token, KEYWORD_SLTIU) == 0) {
      type = lexer::TOKEN_INST_32IM_CP_SLTIU;
    } else if (strcmp(token, KEYWORD_SEQZ) == 0) {
      type = lexer::TOKEN_INST_32IM_CP_SEQZ;
    } else if (strcmp(token, KEYWORD_SNEZ) == 0) {
      type = lexer::TOKEN_INST_32IM_CP_SNEZ;
    } else if (strcmp(token, KEYWORD_SLTZ) == 0) {
      type = lexer::TOKEN_INST_32IM_CP_SLTZ;
    } else if (strcmp(token, KEYWORD_SGTZ) == 0) {
      type = lexer::TOKEN_INST_32IM_CP_SGTZ;
    } else if (strcmp(token, KEYWORD_BEQ) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BEQ;
    } else if (strcmp(token, KEYWORD_BNE) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BNE;
    } else if (strcmp(token, KEYWORD_BGT) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BGT;
    } else if (strcmp(token, KEYWORD_BGE) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BGE;
    } else if (strcmp(token, KEYWORD_BLE) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BLE;
    } else if (strcmp(token, KEYWORD_BLT) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BLT;
    } else if (strcmp(token, KEYWORD_BGTU) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BGTU;
    } else if (strcmp(token, KEYWORD_BGEU) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BGEU;
    } else if (strcmp(token, KEYWORD_BLEU) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BLEU;
    } else if (strcmp(token, KEYWORD_BLTU) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BLTU;
    } else if (strcmp(token, KEYWORD_BEQZ) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BEQZ;
    } else if (strcmp(token, KEYWORD_BNEZ) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BNEZ;
    } else if (strcmp(token, KEYWORD_BLEZ) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BLEZ;
    } else if (strcmp(token, KEYWORD_BGEZ) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BGEZ;
    } else if (strcmp(token, KEYWORD_BLTZ) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BLTZ;
    } else if (strcmp(token, KEYWORD_BGTZ) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_BGTZ;
    } else if (strcmp(token, KEYWORD_J) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_J;
    } else if (strcmp(token, KEYWORD_JAL) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_JAL;
    } else if (strcmp(token, KEYWORD_JR) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_JR;
    } else if (strcmp(token, KEYWORD_JALR) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_JALR;
    } else if (strcmp(token, KEYWORD_CALL) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_CALL;
    } else if (strcmp(token, KEYWORD_RET) == 0) {
      type = lexer::TOKEN_INST_32IM_FC_RET;
    } else if (strcmp(token, KEYWORD_ECALL) == 0) {
      type = lexer::TOKEN_INST_32IM_OS_ECALL;
    } else if (strcmp(token, KEYWORD_EBREAK) == 0) {
      type = lexer::TOKEN_INST_32IM_OS_EBREAK;
    } else if (strcmp(token, KEYWORD_SRET) == 0) {
      type = lexer::TOKEN_INST_32IM_OS_SRET;
    } else if (strcmp(token, KEYWORD_LADD) == 0) {
      type = lexer::TOKEN_INST_32IM_LNS_ADD;
    } else if (strcmp(token, KEYWORD_LSUB) == 0) {
      type = lexer::TOKEN_INST_32IM_LNS_SUB;
    } else if (strcmp(token, KEYWORD_LMUL) == 0) {
      type = lexer::TOKEN_INST_32IM_LNS_MUL;
    } else if (strcmp(token, KEYWORD_LDIV) == 0) {
      type = lexer::TOKEN_INST_32IM_LNS_DIV;
    } else if (strcmp(token, KEYWORD_LSQT) == 0) {
      type = lexer::TOKEN_INST_32IM_LNS_SQT;
    } else if (s_chs > 0) {
      type = lexer::TOKEN_SYMBOL;
    }

    if (type != lexer::TOKEN_NONE) {
      (*tokens)[s_tokens++] = (lexer::RISCVToken){
        .type     = type,
        .line     = line,
        .start    = i,
        .end      = i + s_chs - 1,
        .filename = (char*)filename
      };
      if (type == lexer::TOKEN_SYMBOL) {
        char* string = (char*)malloc((s_chs + 1) * sizeof(char));
        error(FATAL, string == nullptr, "lexer - allocation for string scan returned a NULL pointer", "", __FILE__, __LINE__);
        log("lexer - allocating string ", "", __FILE__, __LINE__);
        strcpy(string, token);
        (*tokens)[s_tokens - 1].lit.string = string;
      }
    }

    if (s_chs == 0)
      s_chs = _lexer_scan_str(*tokens, s_tokens, str, filename, line, i);
    if (s_chs == 0)
      s_chs = _lexer_scan_hexa(*tokens, s_tokens, str, filename, line, i);
    if (s_chs == 0)
      s_chs = _lexer_scan_bin(*tokens, s_tokens, str, filename, line, i);
    if (s_chs == 0)
      s_chs = _lexer_scan_number(*tokens, s_tokens, str, filename, line, i);

    if (s_chs == 0) {
      error(FATAL, true, "lexer - unknown keyword ", "", __FILE__, __LINE__); 
    }

    if constexpr (DEBUG) {
      riscv_token_print(&((*tokens)[s_tokens - 1]));
    }
  }
}

uint32_t _lexer_scan_str(
  lexer::RISCVToken* tokens, uint64_t& s_tokens, char* str,
  const char* filename, const uint32_t line, const uint32_t start
) {
  if (*str != CHAR_QUOTE)
    return 0;

  uint32_t s_string = 1 << 5;
  char* string = (char*)malloc(s_string * sizeof(char));
  error(FATAL, string == nullptr, "lexer - allocation for string scan returned a NULL pointer", "", __FILE__, __LINE__);
  string[0] = CHAR_QUOTE;
  str++;
  log("lexer - allocating string ", "", __FILE__, __LINE__);

  uint32_t s_chs = 1;
  for (; *str != CHAR_QUOTE; s_chs++, str++) {
    if (*str == CHAR_END) {
      free(tokens);
      free(string);
      error(FATAL, true, "lexer - string ends before a ending quote (\")", line, filename, line);
    }

    if (s_chs >= s_string - 2) {
      s_string <<= 1;
      string = (char*)realloc(string, s_string * sizeof(char));
      error(FATAL, string == nullptr, "lexer - reallocation for string scan returned a NULL pointer", "", __FILE__, __LINE__);
      log("lexer - realloced string ", "", __FILE__, __LINE__);
    }
    string[s_chs] = *str;
  }
  string[s_chs++] = CHAR_QUOTE;
  string[s_chs] = CHAR_END;

  tokens[s_tokens++] = (lexer::RISCVToken){
    .type     = lexer::TOKEN_LIT_STRING,
    .lit      = { .string = string },
    .line     = line,
    .start    = start,
    .end      = start + s_chs - 1,
    .filename = (char*)filename
  };

  log("lexer - scanned string ", string, filename, line);
  
  return s_chs;
}

uint32_t _lexer_scan_next(char* token, const uint32_t& max_s_token, char* str, const char* filename, const uint32_t line) {
  uint32_t s_chs = 0;
  log("lexer - scanning next token", "", __FILE__, __LINE__);

  if (*str == CHAR_COMMA) {
    token[0] = CHAR_COMMA;
    token[1] = CHAR_END;
    return 1;
  } else if (*str == CHAR_COLON) {
    token[0] = CHAR_COLON;
    token[1] = CHAR_END;
    return 1;
  } else if (*str == CHAR_LPAREN) {
    token[0] = CHAR_LPAREN;
    token[1] = CHAR_END;
    return 1;
  } else if (*str == CHAR_RPAREN) {
    token[0] = CHAR_RPAREN;
    token[1] = CHAR_END;
    return 1;
  }

  if (!(*str == CHAR_UNDER || *str == CHAR_PERIOD || _lexer_ch_is_alpha(*str))) {
    token[0] = CHAR_END;
    return 0;
  } else {
    token[s_chs++] = *str;
    str++;
  }

  for (; _lexer_ch_is_regex_keyword(*str); token[s_chs++] = *str, str++) {
    if (s_chs >= max_s_token - 2)
      token[max_s_token - 1] = CHAR_END;
    error(ERROR, s_chs >= max_s_token - 2, "lexer - scanned token is too long, this token is probably invalid: ", token, filename, line);
  }

  token[s_chs] = CHAR_END;
  return s_chs;
}

uint32_t _lexer_scan_hexa(
  lexer::RISCVToken* tokens, uint64_t& s_tokens, char* str,
  const char* filename, const uint32_t line, const uint32_t start
) {
  if (str[0] != '0' || (str[1] != 'x' && str[1] != 'X'))
    return 0;
  str += 2;

  log("lexer - scanning hexa number", "", __FILE__, __LINE__);
  uint32_t n = 0, s_chs = 2;
  for (; _lexer_ch_is_hexa(*str); s_chs++, str++) {
    n = (n << 4) + (
      (_lexer_ch_is_digit(*str)) * (uint32_t)(*str - '0') +
      (_lexer_ch_is_upper_hexa(*str)) * ((uint32_t)(*str - 'A') + 10) +
      (_lexer_ch_is_lower_hexa(*str)) * ((uint32_t)(*str - 'a') + 10)
    );
  }
  log("lexer - resulting number ", (int32_t)n, __FILE__, __LINE__);

  tokens[s_tokens++] = (lexer::RISCVToken){
    .type     = lexer::TOKEN_LIT_NUMBER,
    .lit      = { .number = (int32_t)n },
    .line     = line,
    .start    = start,
    .end      = start + s_chs - 1,
    .filename = (char*)filename
  };
  
  return s_chs;
}

uint32_t _lexer_scan_bin(
  lexer::RISCVToken* tokens, uint64_t& s_tokens, char* str,
  const char* filename, const uint32_t line, const uint32_t start
) {
  if (str[0] != '0' || (str[1] != 'b' && str[1] != 'B'))
    return 0;
  str += 2;

  log("lexer - scanning bin number", "", __FILE__, __LINE__);

  uint32_t n = 0, s_chs = 2;
  for (; _lexer_ch_is_bin(*str); s_chs++, str++)
    n = (n << 1) + (uint32_t)(*str == '1');

  log("lexer - resulting number ", (int32_t)n, __FILE__, __LINE__);

  tokens[s_tokens++] = (lexer::RISCVToken){
    .type     = lexer::TOKEN_LIT_NUMBER,
    .lit      = { .number = (int32_t)n },
    .line     = line,
    .start    = start,
    .end      = start + s_chs - 1,
    .filename = (char*)filename
  };
  
  return s_chs;
}

uint32_t _lexer_scan_number(
  lexer::RISCVToken* tokens, uint64_t& s_tokens, char* str,
  const char* filename, const uint32_t line, const uint32_t start
) {
  if (!_lexer_ch_is_digit(*str) && *str != CHAR_MINUS && *str != CHAR_PLUS)
    return 0;

  log("lexer - scanning bin number", "", __FILE__, __LINE__);

  uint8_t sign = 0, pm = *str == CHAR_MINUS || *str == CHAR_PLUS;
  if (pm) {
    sign = *str == CHAR_MINUS ? 1 : 0;
    str++;
  }

  uint32_t n = 0, s_chs = pm;
  for (; _lexer_ch_is_digit(*str); s_chs++, str++)
    n = 10 * n + (uint32_t)(*str - '0');

  log("lexer - resulting number ", ((sign ? -1 : 1) * (int32_t)n), __FILE__, __LINE__);

  tokens[s_tokens++] = (lexer::RISCVToken){
    .type     = lexer::TOKEN_LIT_NUMBER,
    .lit      = { .number = (sign ? -1 : 1) * (int32_t)n },
    .line     = line,
    .start    = start - pm,
    .end      = start + s_chs - 1,
    .filename = (char*)filename
  };
  
  return s_chs;
}

uint32_t _lexer_skip_space(char* str) {
  uint32_t s_chs = 0;
  for (; _lexer_ch_is_space(*str); s_chs++, str++);
  return s_chs;
}

uint32_t _lexer_skip_comments(char* str) {
  if (*str != CHAR_HASH)
    return 0;
  uint32_t s_chs = 0;
  for (; *str != CHAR_END; s_chs++, str++); // getline function cuts the \n
  return s_chs;
}

lexer::RISCVToken* _lexer_tokens_realloc(lexer::RISCVToken* tokens, uint64_t s_tokens, uint64_t& max_s_tokens) {
  if (s_tokens < max_s_tokens)
    return tokens;
  log("lexer - reallocing tokens ", s_tokens, __FILE__, __LINE__);
  max_s_tokens <<= 1;
  tokens = (lexer::RISCVToken*)realloc(tokens, max_s_tokens * sizeof(lexer::riscv_token));
  error(FATAL, tokens == nullptr, "lexer - realloc of token array returned NULL pointer", "", __FILE__, __LINE__);
  return tokens;
}

bool _lexer_ch_is_regex_keyword(const char ch) {
  return ch == CHAR_UNDER || _lexer_ch_is_alpha(ch) || _lexer_ch_is_digit(ch);
}

bool _lexer_ch_is_hexa(const char ch) {
  return _lexer_ch_is_digit(ch) || _lexer_ch_is_lower_hexa(ch) || _lexer_ch_is_upper_hexa(ch);
}

bool _lexer_ch_is_digit(const char ch) {
  return ch >= '0' && ch <= '9';
}

bool _lexer_ch_is_lower_hexa(const char ch) {
  return ch >= 'a' && ch <= 'f';
}

bool _lexer_ch_is_upper_hexa(const char ch) {
  return ch >= 'A' && ch <= 'F';
}

bool _lexer_ch_is_bin(const char ch) {
  return ch == '0' or ch == '1';
}

bool _lexer_ch_is_space(const char ch) {
  return ch == CHAR_NEWLINE || ch == CHAR_SPACE || ch == CHAR_TAB || ch == CHAR_CAR_RET || ch == CHAR_F_FEED || ch == CHAR_VTAB;
}

bool _lexer_ch_is_alpha(const char ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
