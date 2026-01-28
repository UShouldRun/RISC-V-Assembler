#include "parser_private.hpp"

namespace parser {
  RISCVAST* parse(lexer::RISCVToken* tokens, const uint64_t s_tokens) {
    error(FATAL, tokens == nullptr, "parser - tokens is a nullptr", "", __FILE__, __LINE__);

    uint64_t
      max_s_text = 1 << 5,
      max_s_data = 1 << 3;

    RISCVAST* ast = (RISCVAST*)malloc(sizeof(struct riscv_ast) + max_s_text * sizeof(struct riscv_astn_text));
    error(FATAL, ast == nullptr, "parser - allocation of RISCVAST* returned a nullptr", "", __FILE__, __LINE__);
    ast->data  = nullptr;
    ast->error = false;
    ast->s_text = ast->s_data = 0;
    log("parser - initialized ast", "", __FILE__, __LINE__);

    uint64_t i = 0;
    if (tokens[i].type == lexer::TOKEN_TEXT) {
      _parser_parse_text(&ast, max_s_text, tokens, s_tokens, i);
      _parser_parse_data(ast, max_s_data, tokens, s_tokens, i);
    } else if (tokens[i].type == lexer::TOKEN_DATA) {
      _parser_parse_data(ast, max_s_data, tokens, s_tokens, i);
      _parser_parse_text(&ast, max_s_text, tokens, s_tokens, i);
    } else {
      error(
        FATAL, 
        true,
        "parser - grammatical structure of assembly is incorrect: missing/miss placed .text symbol",
        "",
        tokens[i].filename,
        tokens[i].line
      );
    }
    
    log("parser - returning ast", "", __FILE__, __LINE__);
    return ast;
  }

  void check(RISCVAST* ast) {
    for (uint64_t i = 0; i < ast->s_text; i++) {
      const RISCVASTN_Text* cmd = &(ast->text[i]);

      switch (cmd->inst->type) {
        case lexer::TOKEN_INST_32IM_FC_J: {
          const bool error = cmd->f1->type != lexer::TOKEN_SYMBOL;
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_J,
            lexer::riscv_token_get_type_string(lexer::TOKEN_INST_32IM_FC_J),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_JR:
        case lexer::TOKEN_INST_32IM_FC_CALL: {
          const bool error = !lexer::riscv_token_is_reg(cmd->f1->type);
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_JR_CALL,
            lexer::riscv_token_get_type_string(cmd->inst->type),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_JAL: {
          const bool error = !(
            (
              lexer::riscv_token_is_reg(cmd->f1->type) &&
              cmd->f2->type == lexer::TOKEN_LIT_NUMBER
            ) || (
              cmd->f1->type == lexer::TOKEN_SYMBOL &&
              cmd->f2 == nullptr
            )
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_JAL,
            lexer::riscv_token_get_type_string(lexer::TOKEN_INST_32IM_FC_JAL),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }
        case lexer::TOKEN_INST_32IM_FC_JALR: {
          const bool error = !(
            lexer::riscv_token_is_reg(cmd->f1->type) &&
            (
              cmd->f2 == nullptr || 
              (lexer::riscv_token_is_reg(cmd->f2->type) && cmd->f3->type == lexer::TOKEN_LIT_NUMBER)
            )
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_JALR,
            lexer::riscv_token_get_type_string(lexer::TOKEN_INST_32IM_FC_JALR),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_MOVE_LI:
        case lexer::TOKEN_INST_32IM_MOVE_LUI:
        case lexer::TOKEN_INST_32IM_MOVE_AUIPC: {
          const bool error = !(
            lexer::riscv_token_is_reg(cmd->f1->type) &&
            cmd->f2->type == lexer::TOKEN_LIT_NUMBER
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_1,
            lexer::riscv_token_get_type_string(cmd->inst->type),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_MOVE_LA: 
        case lexer::TOKEN_INST_32IM_FC_BEQZ:
        case lexer::TOKEN_INST_32IM_FC_BNEZ:
        case lexer::TOKEN_INST_32IM_FC_BLEZ:
        case lexer::TOKEN_INST_32IM_FC_BGEZ:
        case lexer::TOKEN_INST_32IM_FC_BLTZ:
        case lexer::TOKEN_INST_32IM_FC_BGTZ: {
          const bool error = !(
            lexer::riscv_token_is_reg(cmd->f1->type) &&
            cmd->f2->type == lexer::TOKEN_SYMBOL
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_2,
            lexer::riscv_token_get_type_string(cmd->inst->type),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_MOVE_MV:
        case lexer::TOKEN_INST_32IM_ALS_NEG:
        case lexer::TOKEN_INST_32IM_ALS_NOT: 
        case lexer::TOKEN_INST_32IM_CP_SEQZ:
        case lexer::TOKEN_INST_32IM_CP_SNEZ:
        case lexer::TOKEN_INST_32IM_CP_SLTZ:
        case lexer::TOKEN_INST_32IM_CP_SGTZ:
        case lexer::TOKEN_INST_32IM_LNS_SQT: {
          const bool error = !(
            lexer::riscv_token_is_reg(cmd->f1->type) &&
            lexer::riscv_token_is_reg(cmd->f2->type)
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_3,
            lexer::riscv_token_get_type_string(cmd->inst->type),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_BGT:
        case lexer::TOKEN_INST_32IM_FC_BLE:
        case lexer::TOKEN_INST_32IM_FC_BGTU:
        case lexer::TOKEN_INST_32IM_FC_BLEU: {
          const bool error = !(
            lexer::riscv_token_is_reg(cmd->f1->type) &&
            lexer::riscv_token_is_reg(cmd->f2->type) &&
            cmd->f3->type == lexer::TOKEN_SYMBOL
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_4,
            lexer::riscv_token_get_type_string(cmd->inst->type),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_ADDI:
        case lexer::TOKEN_INST_32IM_ALS_ANDI:
        case lexer::TOKEN_INST_32IM_ALS_ORI:
        case lexer::TOKEN_INST_32IM_ALS_XORI:
        case lexer::TOKEN_INST_32IM_ALS_SLLI:
        case lexer::TOKEN_INST_32IM_ALS_SRLI:
        case lexer::TOKEN_INST_32IM_ALS_SRAI:
        case lexer::TOKEN_INST_32IM_CP_SLTI:
        case lexer::TOKEN_INST_32IM_CP_SLTIU:
        case lexer::TOKEN_INST_32IM_FC_BEQ:
        case lexer::TOKEN_INST_32IM_FC_BNE:
        case lexer::TOKEN_INST_32IM_FC_BGE:
        case lexer::TOKEN_INST_32IM_FC_BLT:
        case lexer::TOKEN_INST_32IM_FC_BGEU:
        case lexer::TOKEN_INST_32IM_FC_BLTU: {
          const bool error = !(
            lexer::riscv_token_is_reg(cmd->f1->type) &&
            lexer::riscv_token_is_reg(cmd->f2->type) &&
            cmd->f3->type == lexer::TOKEN_LIT_NUMBER
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_5,
            lexer::riscv_token_get_type_string(cmd->inst->type),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_ADD:
        case lexer::TOKEN_INST_32IM_ALS_SUB:
        case lexer::TOKEN_INST_32IM_ALS_AND:
        case lexer::TOKEN_INST_32IM_ALS_OR:
        case lexer::TOKEN_INST_32IM_ALS_XOR:
        case lexer::TOKEN_INST_32IM_ALS_SLL:
        case lexer::TOKEN_INST_32IM_ALS_SRL:
        case lexer::TOKEN_INST_32IM_ALS_SRA:
        case lexer::TOKEN_INST_32IM_MD_MUL:
        case lexer::TOKEN_INST_32IM_MD_MULH:
        case lexer::TOKEN_INST_32IM_MD_MULSU:
        case lexer::TOKEN_INST_32IM_MD_MULU:
        case lexer::TOKEN_INST_32IM_MD_DIV:
        case lexer::TOKEN_INST_32IM_MD_DIVU:
        case lexer::TOKEN_INST_32IM_MD_REM:
        case lexer::TOKEN_INST_32IM_MD_REMU:
        case lexer::TOKEN_INST_32IM_CP_SLT:
        case lexer::TOKEN_INST_32IM_CP_SLTU:
        case lexer::TOKEN_INST_32IM_LNS_ADD:
        case lexer::TOKEN_INST_32IM_LNS_SUB:
        case lexer::TOKEN_INST_32IM_LNS_MUL:
        case lexer::TOKEN_INST_32IM_LNS_DIV: {
          const bool error = !(
            lexer::riscv_token_is_reg(cmd->f1->type) &&
            lexer::riscv_token_is_reg(cmd->f2->type) &&
            lexer::riscv_token_is_reg(cmd->f3->type)
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_6,
            lexer::riscv_token_get_type_string(cmd->inst->type),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        case lexer::TOKEN_INST_32IM_LS_LB:
        case lexer::TOKEN_INST_32IM_LS_LH:
        case lexer::TOKEN_INST_32IM_LS_LW:
        case lexer::TOKEN_INST_32IM_LS_LBU:
        case lexer::TOKEN_INST_32IM_LS_LHU:
        case lexer::TOKEN_INST_32IM_LS_SB:
        case lexer::TOKEN_INST_32IM_LS_SH:
        case lexer::TOKEN_INST_32IM_LS_SW: {
          const bool error = !(
            (
              lexer::riscv_token_is_reg(cmd->f1->type) &&
              cmd->f2->type == lexer::TOKEN_SYMBOL &&
              cmd->f3 == nullptr
            ) || (
              lexer::riscv_token_is_reg(cmd->f1->type) &&
              cmd->f2->type == lexer::TOKEN_LIT_NUMBER &&
              lexer::riscv_token_is_reg(cmd->f3->type)
            ) || (
              lexer::riscv_token_is_reg(cmd->f1->type) &&
              cmd->f2->type == lexer::TOKEN_SYMBOL &&
              lexer::riscv_token_is_reg(cmd->f3->type)
            )
          );
          ast->error |= error;
          error(
            ERROR,
            error,
            CHECK_ERROR_MSG_LS,
            lexer::riscv_token_get_type_string(cmd->inst->type),
            cmd->inst->filename,
            cmd->inst->line
          );
          break;
        }

        default: {
          break;
        }
      }
    }
  }

  void ast_print(const RISCVAST* ast) {
    if (ast == nullptr)
      return;
    
    std::cout << "AST {" << std::endl;

    std::cout << (ast->s_data > 0 ? "  Data {" : "") << std::endl;
    for (uint64_t i = 0; i < ast->s_data; i++) {
      std::cout 
        << "    Symbol: " << ast->data[i].symbol->lit.string << ", \n"
        << "      Type: " << lexer::riscv_token_get_type_string(ast->data[i].type->type) << ", \n"
        << "      Values: (";

      for (uint64_t j = 0; j < ast->data[i].s_arr; j++) {
        if (ast->data[i].arr[j]->type == lexer::TOKEN_LIT_NUMBER) {
          std::cout << ast->data[i].arr[j]->lit.number;
        } else {
          std::cout << ast->data[i].arr[j]->lit.string;
        }
        std::cout << (j < ast->data->s_arr - 1 ? ", " : "");
      }
      std::cout << ")" << std::endl;
    }
    std::cout << (ast->s_data > 0 ? "  }" : "") << std::endl;

    error(FATAL, ast->text == nullptr, "parser - ast->text is a nullptr", "", __FILE__, __LINE__);
    std::cout << "  Text {" << std::endl;
    for (uint64_t i = 0; i < ast->s_text; i++) {
      std::cout << "    Inst: " << lexer::riscv_token_get_type_string(ast->text[i].inst->type) << "\n";

      if (ast->text[i].f1 == nullptr)
        continue;

      std::cout << "      f1: " << lexer::riscv_token_get_type_string(ast->text[i].f1->type) << "\n";

      if (ast->text[i].f2 == nullptr)
        continue;

      lexer::RISCVTokenType type = ast->text[i].f2->type;
      bool not_lit_or_symbol = !lexer::riscv_token_is_lit(type) && type != lexer::TOKEN_SYMBOL;
      char* str = type == lexer::TOKEN_LIT_STRING || type == lexer::TOKEN_SYMBOL ? ast->text[i].f2->lit.string : nullptr;
      int32_t number = type == lexer::TOKEN_LIT_NUMBER ? ast->text[i].f2->lit.number : 0;

      std::cout << "      f2: ";
      if (not_lit_or_symbol) {
        std::cout << lexer::riscv_token_get_type_string(type) << "\n";
      } else if (str != nullptr) {
        std::cout << str << "\n";
      } else {
        std::cout << number << "\n";
      }

      if (ast->text[i].f3 == nullptr)
        continue;

      type = ast->text[i].f3->type;
      not_lit_or_symbol = !lexer::riscv_token_is_lit(type) && type != lexer::TOKEN_SYMBOL;
      str = type == lexer::TOKEN_LIT_STRING || type == lexer::TOKEN_SYMBOL ? ast->text[i].f3->lit.string : nullptr;
      number = type == lexer::TOKEN_LIT_NUMBER ? ast->text[i].f3->lit.number : 0;

      std::cout << "      f3: ";
      if (not_lit_or_symbol) {
        std::cout << lexer::riscv_token_get_type_string(type) << "\n";
      } else if (str != nullptr) {
        std::cout << str << "\n";
      } else {
        std::cout << number << "\n";
      }

      if (ast->text[i].f4 == nullptr)
        continue;

      type = ast->text[i].f4->type;
      not_lit_or_symbol = !lexer::riscv_token_is_lit(type) && type != lexer::TOKEN_SYMBOL;
      str = type == lexer::TOKEN_LIT_STRING || type == lexer::TOKEN_SYMBOL ? ast->text[i].f4->lit.string : nullptr;
      number = type == lexer::TOKEN_LIT_NUMBER ? ast->text[i].f4->lit.number : 0;

      std::cout << "      f4: ";
      if (not_lit_or_symbol) {
        std::cout << lexer::riscv_token_get_type_string(type) << "\n";
      } else if (str != nullptr) {
        std::cout << str << "\n";
      } else {
        std::cout << number << "\n";
      }
    }
    std::cout << "  }" << std::endl;
    std::cout << "}" << std::endl;
  }
  
  void ast_free(RISCVAST* ast) {
    if (ast == nullptr)
      return;
    
    if (ast->data != nullptr) {
      for (uint64_t i = 0; i < ast->s_data; i++) {
        if (ast->data[i].arr != nullptr)
          free(ast->data[i].arr);
      }
      free(ast->data);
    }

    free(ast);
  }
}

void _parser_parse_text(
  parser::RISCVAST** ast, uint64_t& max_s_text,
  lexer::RISCVToken* tokens, const uint64_t s_tokens, uint64_t& i
) {
  error(
    FATAL, 
    tokens[i].type != lexer::TOKEN_TEXT,
    "parser - grammatical structure of assembly is incorrect: missing .text symbol",
    "",
    tokens[i].filename,
    tokens[i].line
  );

  parser::RISCVAST* _ast = *ast;
  log("parser - parsing .text", "", __FILE__, __LINE__);

  i++;
  for (uint64_t incr = 0; i < s_tokens; i += incr) {
    if (tokens[i].type == lexer::TOKEN_DATA) {
      error(FATAL, _ast->data != nullptr, "parser - already parsed .data section", "", tokens[i].filename, tokens[i].line);
      return;
    }

    if (_ast->s_text >= max_s_text) {
      max_s_text <<= 1;
      _ast = (parser::RISCVAST*)realloc(_ast, sizeof(parser::riscv_ast) + max_s_text * sizeof(parser::riscv_astn_text));
      error(FATAL, _ast == nullptr, "parser - reallocation of RISCVAST* returned a nullptr", "", __FILE__, __LINE__);
      log("parser - reallocated text array", "", __FILE__, __LINE__);
    }

    incr = 1;
    switch (tokens[i].type) {
      case lexer::TOKEN_SYMBOL: {
        _ast->error |= i + 1 >= s_tokens || tokens[i + 1].type != lexer::TOKEN_COLON;
        error(
          FATAL, 
          _ast->error,
          "parser - following character is missing \":\": ",
          tokens[i].lit.string,
          tokens[i].filename,
          tokens[i].line
        );

        if (!_ast->error) {
          _ast->text[_ast->s_text++] = (parser::RISCVASTN_Text){
            .inst   = &(tokens[i]),
            .f1     = nullptr,
            .f2     = nullptr,
            .f3     = nullptr,
            .f4     = nullptr
          };
          incr = 2;
          log("parser - parsed TOKEN_SYMBOL rule ", tokens[i].lit.string, tokens[i].filename, tokens[i].line);
        }

        break;
      }

      case lexer::TOKEN_INST_32IM_NOP:
      case lexer::TOKEN_INST_32IM_OS_ECALL:
      case lexer::TOKEN_INST_32IM_OS_EBREAK:
      case lexer::TOKEN_INST_32IM_OS_SRET: 
      case lexer::TOKEN_INST_32IM_FC_RET: {
        _ast->text[_ast->s_text++] = (parser::RISCVASTN_Text){
          .inst   = &(tokens[i]),
          .f1     = nullptr,
          .f2     = nullptr,
          .f3     = nullptr,
          .f4     = nullptr
        };
        incr = 1;
        log(
          "parser - parsed zero arg instruction rule ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );
        break;
      }

      case lexer::TOKEN_INST_32IM_FC_J:
      case lexer::TOKEN_INST_32IM_FC_JR:
      case lexer::TOKEN_INST_32IM_FC_CALL: {
        _ast->error |= i + 1 >= s_tokens || lexer::riscv_token_is_inst(tokens[i + 1].type);
        error(
          FATAL,
          _ast->error,
          "parser - the following instruction requires a parameter: ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!_ast->error) {
          _ast->text[_ast->s_text++] = (parser::RISCVASTN_Text){
            .inst   = &(tokens[i]),
            .f1     = &(tokens[i + 1]),
            .f2     = nullptr,
            .f3     = nullptr,
            .f4     = nullptr
          };
          incr = 2;
          log(
            "parser - parsed one arg instruction rule ",
            lexer::riscv_token_get_type_string(tokens[i].type),
            tokens[i].filename,
            tokens[i].line
          );
        }

        break;
      }

      case lexer::TOKEN_INST_32IM_FC_JAL:
      case lexer::TOKEN_INST_32IM_FC_JALR: {
        _ast->error |= (
          !(
            i + 3 <= s_tokens &&
            lexer::riscv_token_is_reg(tokens[i + 1].type) &&
            tokens[i + 2].type == lexer::TOKEN_COMMA &&
            tokens[i + 3].type == lexer::TOKEN_LIT_NUMBER
          ) &&
          !(
            i + 1 < s_tokens &&
            tokens[i + 1].type == lexer::TOKEN_SYMBOL
          )
        );
        error(
          FATAL,
          _ast->error,
          "parser - the following instruction requires either a register and an immediate or a symbol: ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!_ast->error) {
          incr = tokens[i + 3].type == lexer::TOKEN_LIT_NUMBER ? 4 : 2;
          _ast->text[_ast->s_text++] = (parser::RISCVASTN_Text){
            .inst   = &(tokens[i]),
            .f1     = &(tokens[i + 1]),
            .f2     = incr == 4 ? &(tokens[i + 3]) : nullptr,
            .f3     = nullptr,
            .f4     = nullptr
          };
          log(
            "parser - parsed one/two arg instruction rule ",
            lexer::riscv_token_get_type_string(tokens[i].type),
            tokens[i].filename,
            tokens[i].line
          );
        }

        break;
      }

      case lexer::TOKEN_INST_32IM_MOVE_LI:
      case lexer::TOKEN_INST_32IM_MOVE_LA:
      case lexer::TOKEN_INST_32IM_MOVE_LUI:
      case lexer::TOKEN_INST_32IM_MOVE_AUIPC:
      case lexer::TOKEN_INST_32IM_MOVE_MV:
      case lexer::TOKEN_INST_32IM_ALS_NEG:
      case lexer::TOKEN_INST_32IM_ALS_NOT:
      case lexer::TOKEN_INST_32IM_CP_SEQZ:
      case lexer::TOKEN_INST_32IM_CP_SNEZ:
      case lexer::TOKEN_INST_32IM_CP_SLTZ:
      case lexer::TOKEN_INST_32IM_CP_SGTZ:
      case lexer::TOKEN_INST_32IM_FC_BEQZ:
      case lexer::TOKEN_INST_32IM_FC_BNEZ:
      case lexer::TOKEN_INST_32IM_FC_BLEZ:
      case lexer::TOKEN_INST_32IM_FC_BGEZ:
      case lexer::TOKEN_INST_32IM_FC_BLTZ:
      case lexer::TOKEN_INST_32IM_FC_BGTZ:
      case lexer::TOKEN_INST_32IM_LNS_SQT: {
        _ast->error |= (
          i + 3 >= s_tokens ||
          !lexer::riscv_token_is_param(tokens[i + 1].type) ||
          tokens[i + 2].type != lexer::TOKEN_COMMA ||
          !lexer::riscv_token_is_param(tokens[i + 3].type)
        );
        error(
          FATAL,
          _ast->error,
          "parser - the following instruction requires two parameters separated by a comma: ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!_ast->error) {
          _ast->text[_ast->s_text++] = (parser::RISCVASTN_Text){
            .inst   = &(tokens[i]),
            .f1     = &(tokens[i + 1]),
            .f2     = &(tokens[i + 3]),
            .f3     = nullptr,
            .f4     = nullptr
          };
          incr = 4;
          log(
            "parser - parsed two arg instruction rule ",
            lexer::riscv_token_get_type_string(tokens[i].type),
            tokens[i].filename,
            tokens[i].line
          );
        }
        
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
      case lexer::TOKEN_INST_32IM_LNS_ADD:
      case lexer::TOKEN_INST_32IM_LNS_SUB:
      case lexer::TOKEN_INST_32IM_LNS_MUL:
      case lexer::TOKEN_INST_32IM_LNS_DIV: {
        _ast->error |= (
          i + 5 >= s_tokens ||
          !lexer::riscv_token_is_param(tokens[i + 1].type) ||
          tokens[i + 2].type != lexer::TOKEN_COMMA ||
          !lexer::riscv_token_is_param(tokens[i + 3].type) ||
          tokens[i + 4].type != lexer::TOKEN_COMMA ||
          !lexer::riscv_token_is_param(tokens[i + 5].type)
        );
        error(
          FATAL,
          _ast->error,
          "parser - the following instruction requires three parameters separated by commas: ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!_ast->error) {
          _ast->text[_ast->s_text++] = (parser::RISCVASTN_Text){
            .inst   = &(tokens[i]),
            .f1     = &(tokens[i + 1]),
            .f2     = &(tokens[i + 3]),
            .f3     = &(tokens[i + 5]),
            .f4     = nullptr
          };
          incr = 6;
          log(
            "parser - parsed three arg instruction rule ",
            lexer::riscv_token_get_type_string(tokens[i].type),
            tokens[i].filename,
            tokens[i].line
          );
        }
        
        break;
      }      

      case lexer::TOKEN_INST_32IM_LS_LB:
      case lexer::TOKEN_INST_32IM_LS_LH:
      case lexer::TOKEN_INST_32IM_LS_LW:
      case lexer::TOKEN_INST_32IM_LS_LBU:
      case lexer::TOKEN_INST_32IM_LS_LHU:
      case lexer::TOKEN_INST_32IM_LS_SB:
      case lexer::TOKEN_INST_32IM_LS_SH:
      case lexer::TOKEN_INST_32IM_LS_SW: {
        const bool symbol = (
          i + 3 < s_tokens &&
          lexer::riscv_token_is_param(tokens[i + 1].type) &&
          tokens[i + 2].type == lexer::TOKEN_COMMA &&
          tokens[i + 3].type == lexer::TOKEN_SYMBOL
        );
        const bool no_offset = (
          i + 3 < s_tokens &&
          lexer::riscv_token_is_param(tokens[i + 1].type) &&
          tokens[i + 2].type == lexer::TOKEN_COMMA &&
          (lexer::riscv_token_is_param(tokens[i + 3].type) && tokens[i + 3].type != lexer::TOKEN_LIT_NUMBER)
        );
        const bool offset = (
          i + 6 < s_tokens &&
          lexer::riscv_token_is_param(tokens[i + 1].type) &&
          tokens[i + 2].type == lexer::TOKEN_COMMA &&
          tokens[i + 3].type == lexer::TOKEN_LIT_NUMBER &&
          tokens[i + 4].type == lexer::TOKEN_LPAREN &&
          lexer::riscv_token_is_param(tokens[i + 5].type) &&
          tokens[i + 6].type == lexer::TOKEN_RPAREN
        );

        _ast->error |= !symbol && !offset && !no_offset;
        error(
          FATAL,
          _ast->error,
          "parser - the following instruction requires two/three parameters as \"<inst> <xd>, <symbol> || <inst> <xd>, <imm>(<xa>)\": ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );

        if (!_ast->error) {
          if (offset) {
            _ast->text[_ast->s_text++] = (parser::RISCVASTN_Text){
              .inst   = &(tokens[i]),
              .f1     = &(tokens[i + 1]),
              .f2     = &(tokens[i + 3]),
              .f3     = &(tokens[i + 5]),
              .f4     = nullptr
            };
            incr = 7;
          } else {
            _ast->text[_ast->s_text++] = (parser::RISCVASTN_Text){
              .inst   = &(tokens[i]),
              .f1     = &(tokens[i + 1]),
              .f2     = &(tokens[i + 3]),
              .f3     = nullptr,
              .f4     = nullptr
            };
            incr = 4;
          }
          log(
            "parser - parsed load/store instruction rule ",
            lexer::riscv_token_get_type_string(tokens[i].type),
            tokens[i].filename,
            tokens[i].line
          );
        }
        
        break;
      }

      default: {
        error(
          FATAL,
          true,
          "parser - invalid grammatical structure in .text: did not start with a supported instruction token ",
          lexer::riscv_token_get_type_string(tokens[i].type),
          tokens[i].filename,
          tokens[i].line
        );
        break;
      }
    }
  }

  *ast = _ast;
}

void _parser_parse_data(
  parser::RISCVAST* ast, uint64_t& max_s_data,
  lexer::RISCVToken* tokens, const uint64_t s_tokens, uint64_t& i
) {
  if (tokens[i++].type != lexer::TOKEN_DATA)
    return;

  ast->data = (parser::RISCVASTN_Data*)malloc(max_s_data * sizeof(parser::riscv_astn_data));
  error(FATAL, ast->data == nullptr, "parser - allocation of RISCVASTN_Data* returned a nullptr", "", __FILE__, __LINE__);
  
  while (i < s_tokens && tokens[i].type != lexer::TOKEN_TEXT) {
    if (ast->s_data >= max_s_data) {
      max_s_data <<= 1;
      ast->data = (parser::RISCVASTN_Data*)realloc(ast->data, max_s_data * sizeof(parser::riscv_astn_data));
      error(FATAL, ast->data == nullptr, "parser - reallocation of RISCVASTN_Data* returned a nullptr", "", __FILE__, __LINE__);
    }

    const char
      *symbol      = tokens[i].lit.string,
      *filename    = tokens[i].filename;
    uint32_t line = tokens[i].line;
    log(
      "parser - parsing symbol ",
      symbol,
      filename,
      line
    );

    error(
      FATAL,
      !(
        i + 3 < s_tokens && 
        tokens[i].type == lexer::TOKEN_SYMBOL &&
        tokens[i + 1].type == lexer::TOKEN_COLON &&
        lexer::riscv_token_is_data_type(tokens[i + 2].type) &&
        lexer::riscv_token_is_lit(tokens[i + 3].type)
      ),
      "parser - invalid grammatical structure in .data: did not follow the convention \"<symbol>: .<type> <data>\"",
      "",
      tokens[i].filename,
      tokens[i].line
    );

    uint64_t max_s_arr = 1 << 2;
    uint64_t j = ast->s_data;
    ast->data[ast->s_data++] = (parser::RISCVASTN_Data){
      .s_arr      = 0,
      .symbol     = &(tokens[i]),
      .type       = &(tokens[i + 2]),
      .arr        = (lexer::RISCVToken**)malloc(max_s_arr * sizeof(lexer::RISCVToken*))
    };
    i += 3;

    while (i < s_tokens && lexer::riscv_token_is_lit(tokens[i].type)) {
      if (ast->data[j].s_arr >= max_s_arr) {
        max_s_arr <<= 1;
        ast->data[j].arr = (lexer::RISCVToken**)realloc(
          ast->data[j].arr,
          max_s_arr * sizeof(lexer::RISCVToken*)
        );
        error(
          FATAL,
          ast->data[j].arr == nullptr,
          "parser - reallocation of RISCVASTN_Data* returned a nullptr",
          "",
          __FILE__,
          __LINE__
        );
      }

      ast->data[j].arr[ast->data[j].s_arr++] = &(tokens[i]);
      error(
        ERROR,
        lexer::riscv_token_is_lit(tokens[i + 1].type),
        "parser - invalid grammatical structre in .data: two consecutive literals not separated by a comma",
        "",
        tokens[i].filename,
        tokens[i].line
      );
      i += 1 + (tokens[i + 1].type == lexer::TOKEN_COMMA);
    }

    if (ast->data[j].s_arr != max_s_arr) {
      ast->data[j].arr = (lexer::RISCVToken**)realloc(ast->data[j].arr, ast->data[j].s_arr * sizeof(lexer::RISCVToken*));
      error(FATAL, ast->data->arr == nullptr, "parser - final reallocation of RISCVASTN_Data* returned a nullptr", "", __FILE__, __LINE__);
    }

    log(
      "parser - parsed symbol ",
      symbol,
      filename,
      line
    );
  }

  error(FATAL, ast->data == nullptr, "parser - ast->data is a nullptr at the end of the .data function", "", __FILE__, __LINE__);
}
