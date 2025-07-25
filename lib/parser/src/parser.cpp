#include "parser_private.hpp"

namespace parser {
  RISCVAST* parse(const lexer::RISCVToken* tokens, const uint64_t s_tokens) {
    error(FATAL, tokens == nullptr, "parser - tokens is a nullptr", "", __FILE__, __LINE__);

    uint64_t
      max_s_text = 1 << 5, s_text = 0
      max_s_data = 1 << 3, s_data = 0;

    RISCVAST* ast = (RISCVAST*)malloc(sizeof(struct riscv_ast) + max_s_text * sizeof(struct riscv_astn_text));
    error(FATAL, ast == nullptr, "parser - allocation of RISCVAST* returned a nullptr", "", __FILE__, __LINE__);
    ast->data  = nullptr;
    ast->error = false;

    uint64_t i = 0;
    if (tokens[i].type == lexer::TOKEN_TEXT) {
      _parser_parse_text(ast, max_s_text, tokens, s_tokens, i);
      _parser_parse_data(ast, max_s_data, tokens, s_tokens, i);
    } else if (tokens[i].type == lexer::TOKEN_DATA) {
      _parser_parse_data(ast, max_s_data, tokens, s_tokens, i);
      _parser_parse_text(ast, max_s_text, tokens, s_tokens, i);
    } else {
      error(
        FATAL, 
        true,
        "parser - grammatical structure of assembly is incorrect: missing .text label",
        tokens[i].filename,
        tokens[i].line
      );
    }
    
    return ast;
  }

  void rewrite_pseudo(RISCVAST* ast) {
    
  }
}

void _parser_parse_text(
  parser::RISCVAST* ast, uint64_t& max_s_text,
  const lexer::RISCVToken* tokens, const uint64_t s_tokens, uint64_t& i
) {
  error(
    FATAL, 
    tokens[i].type != lexer::TOKEN_TEXT,
    "parser - grammatical structure of assembly is incorrect: missing .text label",
    tokens[i].filename,
    tokens[i].line
  );

  for (uint64_t incr = 0, i++; i < s_tokens && tokens[i].type != lexer::TOKEN_DATA; i += incr) {
    if (ast->s_text >= max_s_text) {
      max_s_text <<= 1;
      ast = (parser::RISCVAST*)realloc(ast, sizeof(parser::riscv_ast) + max_s_text * sizeof(parser::riscv_astn_text));
      error(FATAL, ast == nullptr, "parser - reallocation of RISCVAST* returned a nullptr", "", __FILE__, __LINE__);
    }

    switch (tokens[i].type) {
      case lexer::TOKEN_SYMBOL: {
        ast->error = i + 1 >= s_tokens || tokens[i + 1].type != lexer::TOKEN_COLON;
        error(
          ERROR, 
          ast->error,
          "parser - following label is missing \":\": ",
          tokens[i].lit.string,
          filename,
          line
        );

        if (!ast->error) {
          ast->text[ast->s_text++] = (parser::RISCVASTN_Text){
            .inst = &(tokens[i]),
            .f1   = nullptr,
            .f2   = nullptr,
            .f3   = nullptr,
            .f4   = nullptr
          };
        }

        incr = 2;
        break;
      }

      case lexer::TOKEN_INST_32IM_NOP:
      case lexer::TOKEN_INST_32IM_OS_ECALL:
      case lexer::TOKEN_INST_32IM_OS_EBREAK:
      case lexer::TOKEN_INST_32IM_OS_SRET: 
      case lexer::TOKEN_INST_32IM_FC_RET: {
        ast->text[ast->s_text++] = (parser::RISCVASTN_Text){
          .inst = &(tokens[i]),
          .f1   = nullptr,
          .f2   = nullptr,
          .f3   = nullptr,
          .f4   = nullptr
        };
        incr = 1;
        break;
      }

      case lexer::TOKEN_INST_32IM_FC_J:
      case lexer::TOKEN_INST_32IM_FC_JR:
      case lexer::TOKEN_INST_32IM_FC_CALL: {
        ast->error = i + 1 >= s_tokens || lexer::token_is_inst(tokens[i + 1]);
        error(
          ERROR,
          ast->error,
          "parser - the following instruction requires a parameter: ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!ast->error) {
          ast->text[ast->s_text++] = (parser::RISCVASTN_Text){
            .inst = &(tokens[i]),
            .f1   = &(tokens[i + 1]),
            .f2   = nullptr,
            .f3   = nullptr,
            .f4   = nullptr
          };
        }

        incr = 2;
        break;
      }

      case lexer::TOKEN_INST_32IM_FC_JAL:
      case lexer::TOKEN_INST_32IM_FC_JALR: {

        break;
      }

      case lexer::TOKEN_INST_32IM_MOVE_LI:
      case lexer::TOKEN_INST_32IM_MOVE_LA:
      case lexer::TOKEN_INST_32IM_MOVE_LUI:
      case lexer::TOKEN_INST_32IM_MOVE_AUIPC:
      case lexer::TOKEN_INST_32IM_MOVE_MV:
      case lexer::TOKEN_INST_32IM_ALS_NEG:
      case lexer::TOKEN_INST_32IM_ALS_NOT:
      case lexer::TOKEN_INST_32IM_LS_L:
      case lexer::TOKEN_INST_32IM_CP_SEQZ:
      case lexer::TOKEN_INST_32IM_CP_SNEZ:
      case lexer::TOKEN_INST_32IM_CP_SLTZ:
      case lexer::TOKEN_INST_32IM_CP_SGTZ:
      case lexer::TOKEN_INST_32IM_LNS_SQT: {
        ast->error = (
          i + 3 >= s_tokens ||
          !lexer::riscv_token_is_param(tokens[i + 1]) ||
          tokens[i + 2] != lexer::TOKEN_COMMA ||
          !lexer::token_is_param(tokens[i + 3])
        );
        error(
          ERROR,
          ast->error,
          "parser - the following instruction requires two parameters separated by a comma: ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!ast->error) {
          ast->text[ast->s_text++] = (parser::RISCVASTN_Text){
            .inst = &(tokens[i]),
            .f1   = &(tokens[i + 1]),
            .f2   = &(tokens[i + 3]),
            .f3   = nullptr,
            .f4   = nullptr
          };        
        }
        
        incr = 4;
        break;
      }

      case lexer::TOKEN_INST_32IM_ALS_ADD:
      case lexer::TOKEN_INST_32IM_ALS_ADDI:
      case lexer::TOKEN_INST_32IM_ALS_SUB:
      case lexer::TOKEN_INST_32IM_ALS_AND:
      case lexer::TOKEN_INST_32IM_ALS_ANDI:
      case lexer::TOKEN_INST_32IM_ALS_OR:
      case lexer::TOKEN_INST_32IM_ALS_ORI:
      case lexer::TOKEN_INST_32IM_ALS_XOR:
      case lexer::TOKEN_INST_32IM_ALS_XORI:
      case lexer::TOKEN_INST_32IM_ALS_SLL:
      case lexer::TOKEN_INST_32IM_ALS_SLLI:
      case lexer::TOKEN_INST_32IM_ALS_SRL:
      case lexer::TOKEN_INST_32IM_ALS_SRLI:
      case lexer::TOKEN_INST_32IM_ALS_SRA:
      case lexer::TOKEN_INST_32IM_ALS_SRAI:
      case lexer::TOKEN_INST_32IM_MD_MUL:
      case lexer::TOKEN_INST_32IM_MD_MULH:
      case lexer::TOKEN_INST_32IM_MD_MULSU:
      case lexer::TOKEN_INST_32IM_MD_MULU:
      case lexer::TOKEN_INST_32IM_MD_DIV:
      case lexer::TOKEN_INST_32IM_MD_DIVU:
      case lexer::TOKEN_INST_32IM_MD_REM:
      case lexer::TOKEN_INST_32IM_MD_REMU:
      case lexer::TOKEN_INST_32IM_CP_SLT:
      case lexer::TOKEN_INST_32IM_CP_SLTI:
      case lexer::TOKEN_INST_32IM_CP_SLTU:
      case lexer::TOKEN_INST_32IM_CP_SLTIU:
      case lexer::TOKEN_INST_32IM_FC_BEQ:
      case lexer::TOKEN_INST_32IM_FC_BNE:
      case lexer::TOKEN_INST_32IM_FC_BGT:
      case lexer::TOKEN_INST_32IM_FC_BGE:
      case lexer::TOKEN_INST_32IM_FC_BLE:
      case lexer::TOKEN_INST_32IM_FC_BLT:
      case lexer::TOKEN_INST_32IM_FC_BGTU:
      case lexer::TOKEN_INST_32IM_FC_BGEU:
      case lexer::TOKEN_INST_32IM_FC_BLTU:
      case lexer::TOKEN_INST_32IM_FC_BLEU:
      case lexer::TOKEN_INST_32IM_FC_BEQZ:
      case lexer::TOKEN_INST_32IM_FC_BNEZ:
      case lexer::TOKEN_INST_32IM_FC_BLEZ:
      case lexer::TOKEN_INST_32IM_FC_BGEZ:
      case lexer::TOKEN_INST_32IM_FC_BLTZ:
      case lexer::TOKEN_INST_32IM_FC_BGTZ:
      case lexer::TOKEN_INST_32IM_LNS_ADD:
      case lexer::TOKEN_INST_32IM_LNS_SUB:
      case lexer::TOKEN_INST_32IM_LNS_MUL:
      case lexer::TOKEN_INST_32IM_LNS_DIV: {
        ast->error = (
          i + 5 >= s_tokens ||
          !lexer::riscv_token_is_param(tokens[i + 1]) ||
          tokens[i + 2] != lexer::TOKEN_COMMA ||
          !lexer::token_is_param(tokens[i + 3]) ||
          tokens[i + 4] != lexer::TOKEN_COMMA ||
          !lexer::token_is_param(tokens[i + 5])
        );
        error(
          ERROR,
          ast->error,
          "parser - the following instruction requires three parameters separated by commas: ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!ast->error) {
          ast->text[ast->s_text++] = (parser::RISCVASTN_Text){
            .inst = &(tokens[i]),
            .f1   = &(tokens[i + 1]),
            .f2   = &(tokens[i + 3]),
            .f3   = &(tokens[i + 5]),
            .f4   = nullptr
          };
        }
        
        incr = 6;
        break;
      }      

      case lexer::TOKEN_INST_32IM_LS_LB:
      case lexer::TOKEN_INST_32IM_LS_LH:
      case lexer::TOKEN_INST_32IM_LS_LW:
      case lexer::TOKEN_INST_32IM_LS_LBU:
      case lexer::TOKEN_INST_32IM_LS_LHU:
      case lexer::TOKEN_INST_32IM_LS_S:
      case lexer::TOKEN_INST_32IM_LS_SB:
      case lexer::TOKEN_INST_32IM_LS_SH:
      case lexer::TOKEN_INST_32IM_LS_SW: {
        ast->error = (
          i + 6 >= s_tokens ||
          !lexer::riscv_token_is_param(tokens[i + 1]) ||
          tokens[i + 2] != lexer::TOKEN_COMMA ||
          tokens[i + 3].type != lexer::TOKEN_LIT_NUMBER ||
          tokens[i + 4] != lexer::TOKEN_LPAREN ||
          !lexer::token_is_param(tokens[i + 5]) ||
          tokens[i + 6] != lexer::TOKEN_LPAREN ||
        );
        error(
          ERROR,
          ast->error,
          "parser - the following instruction requires three parameters as \"<inst> <param1>, <imm>(<param2>)\": ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!ast->error) {
          ast->text[ast->s_text++] = (parser::RISCVASTN_Text){
            .inst = &(tokens[i]),
            .f1   = &(tokens[i + 1]),
            .f2   = &(tokens[i + 3]),
            .f3   = &(tokens[i + 5]),
            .f4   = nullptr
          };
        }
        
        incr = 6;
        break;
      }

      default: {
        error(
          true,
          "parser - invalid grammatical structure in .text: did not start with a supported instruction token",
          tokens[i].filename,
          tokens[i].line
        );
        break;
      }
    }
  }
}

void _parser_parse_data(
  parser::RISCVAST* ast, uint64_t& max_s_text,
  const lexer::RISCVToken* tokens, const uint64_t s_tokens, uint64_t& i
) {
  if (tokens[i].type != lexer::TOKEN_DATA)
    return;

}
