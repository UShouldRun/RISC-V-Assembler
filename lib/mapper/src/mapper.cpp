#include "mapper_private.hpp"

namespace mapper {
  uint32_t* map_inst2bin(const RISCVAST* ast, uint64_t& s_insts) {
    error(FATAL, ast == nullptr, "mapper - ast is a nullptr in map_inst2bin", "", __FILE__, __LINE__);

    std::unordered_map<const char*, const uint32_t> map;

    for (uint64_t i = 0; i < ast->s_data; i++)
      map.insert({ ast->data[i].symbol->lit.string, i << 2 });

    for (uint64_t i = 0; i < ast->s_text; i++) {
      if (ast->text[i].inst.type != lexer::TOKEN_SYMBOL)
        continue;
      map.insert({ ast->text[i].inst->lit.string, i << 2 });
    }
    
    uint64_t max_s_insts = ast->s_text >= 4 ? ast->s_text : 4;
    uint32_t* insts = (uint32_t*)malloc(max_s_insts * sizeof(uint32_t));
    error(FATAL, insts == nullptr, "mapper - allocation of instruction array returned a nullptr", "", __FILE__, __LINE__);

    for (uint64_t i = 0; i < ast->s_text; i++) {
      if (s_insts + 1 >= max_s_insts) { // In case we get a pseudo instruction that needs 2 instructions
        max_s_insts += max_s_insts >> 2;
        insts = (uint32_t*)realloc(insts, max_s_insts * sizeof(uint32_t));
        error(FATAL, insts == nullptr, "mapper - reallocation of instruction array returned a nullptr", "", __FILE__, __LINE__);
      }

      lexer::RISCVToken* inst = &(ast->text[i]);

      OpType optype = OPTYPE_NONE;
      uint8_t 
        opcode = 0x00,
        funct3 = 0x0,
        funct7 = 0x00;

      switch (inst->type) {
        case TOKEN_INST_32IM_NOP: {
          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            OPCODE_ADDI
          );
          continue;
        }

        case TOKEN_INST_32IM_MOVE_LA: {
          insts[s_insts++] = riscv_map_u_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_i_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_ADDI
          );
          continue;
        }

        case TOKEN_INST_32IM_MOVE_LI: {
          insts[s_insts++] = riscv_map_i_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_ADDI
          );
          if (inst->f2->lit.number <= 0x00000FFF)
            continue;

          insts[s_insts++] = riscv_map_u_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_LUI
          );
          continue;
        }

        case TOKEN_INST_32IM_MOVE_LUI: {
          optype = OPTYPE_U;
          opcode = OPCODE_LUI;
          break;
        }

        case TOKEN_INST_32IM_MOVE_AUIPC: {
          optype = OPTYPE_U;
          opcode = OPCODE_AUIPC;
          break;
        }

        case TOKEN_INST_32IM_MOVE_MV: {
          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(inst->f2->type),
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_ADDI
          );
          continue;
        }

        case TOKEN_INST_32IM_ALS_NEG: {
          insts[s_insts++] = riscv_map_r_type(
            0x20,
            lexer::riscv_token_get_reg(inst->f2->type),
            lexer::riscv_token_get_reg(lexer::TOKEN_X0),
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_SUB
          );
          continue;
        }

        case TOKEN_INST_32IM_ALS_ADD: {
          optype = OPTYPE_R;
          opcode = OPCODE_ADD;
          funct3 = 0x0;
          funct7 = 0x00;
          break;
        }

        case TOKEN_INST_32IM_ALS_ADDI: {
          optype = OPTYPE_I;
          opcode = OPCODE_ADDI;
          funct3 = 0x0;
          break;
        }

        case TOKEN_INST_32IM_ALS_SUB: {
          optype = OPTYPE_R;
          opcode = OPCODE_SUB;
          funct3 = 0x0;
          funct7 = 0x20;
          break;
        }

        case TOKEN_INST_32IM_ALS_NOT: {
          insts[s_insts++] = riscv_map_i_type(
            0xFFF,
            lexer::riscv_token_get_reg(inst->f2->type),
            0x4,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_XORI
          );
          continue;
        }

        case TOKEN_INST_32IM_ALS_AND: {
          optype = ;
          opcode = ;
          funct3 = ;
          funct7 = ;
          break;
        }

        case TOKEN_INST_32IM_ALS_ANDI:
        case TOKEN_INST_32IM_ALS_OR:
        case TOKEN_INST_32IM_ALS_ORI:
        case TOKEN_INST_32IM_ALS_XOR:
        case TOKEN_INST_32IM_ALS_XORI:
        case TOKEN_INST_32IM_ALS_SLL:
        case TOKEN_INST_32IM_ALS_SLLI:
        case TOKEN_INST_32IM_ALS_SRL:
        case TOKEN_INST_32IM_ALS_SRLI:
        case TOKEN_INST_32IM_ALS_SRA:
        case TOKEN_INST_32IM_ALS_SRAI:

        case TOKEN_INST_32IM_MD_MUL:
        case TOKEN_INST_32IM_MD_MULH:
        case TOKEN_INST_32IM_MD_MULSU:
        case TOKEN_INST_32IM_MD_MULU:
        case TOKEN_INST_32IM_MD_DIV:
        case TOKEN_INST_32IM_MD_DIVU:
        case TOKEN_INST_32IM_MD_REM:
        case TOKEN_INST_32IM_MD_REMU:

        case TOKEN_INST_32IM_LS_LB:
        case TOKEN_INST_32IM_LS_LH:
        case TOKEN_INST_32IM_LS_LW:
        case TOKEN_INST_32IM_LS_LBU:
        case TOKEN_INST_32IM_LS_LHU:
        case TOKEN_INST_32IM_LS_SB:
        case TOKEN_INST_32IM_LS_SH:
        case TOKEN_INST_32IM_LS_SW:

        case TOKEN_INST_32IM_CP_SLT:
        case TOKEN_INST_32IM_CP_SLTI:
        case TOKEN_INST_32IM_CP_SLTU:
        case TOKEN_INST_32IM_CP_SLTIU:
        case TOKEN_INST_32IM_CP_SEQZ:
        case TOKEN_INST_32IM_CP_SNEZ:
        case TOKEN_INST_32IM_CP_SLTZ:
        case TOKEN_INST_32IM_CP_SGTZ:

        case TOKEN_INST_32IM_FC_BEQ:
        case TOKEN_INST_32IM_FC_BNE:
        case TOKEN_INST_32IM_FC_BGT:
        case TOKEN_INST_32IM_FC_BGE:
        case TOKEN_INST_32IM_FC_BLE:
        case TOKEN_INST_32IM_FC_BLT:
        case TOKEN_INST_32IM_FC_BGTU:
        case TOKEN_INST_32IM_FC_BGEU:
        case TOKEN_INST_32IM_FC_BLTU:
        case TOKEN_INST_32IM_FC_BLEU:
        case TOKEN_INST_32IM_FC_BEQZ:
        case TOKEN_INST_32IM_FC_BNEZ:
        case TOKEN_INST_32IM_FC_BLEZ:
        case TOKEN_INST_32IM_FC_BGEZ:
        case TOKEN_INST_32IM_FC_BLTZ:
        case TOKEN_INST_32IM_FC_BGTZ:
        case TOKEN_INST_32IM_FC_J:
        case TOKEN_INST_32IM_FC_JAL:
        case TOKEN_INST_32IM_FC_JR:
        case TOKEN_INST_32IM_FC_JALR:
        case TOKEN_INST_32IM_FC_CALL:
        case TOKEN_INST_32IM_FC_RET:

        case TOKEN_INST_32IM_OS_ECALL:
        case TOKEN_INST_32IM_OS_EBREAK:
        case TOKEN_INST_32IM_OS_SRET:

        case TOKEN_INST_32IM_LNS_ADD:
        case TOKEN_INST_32IM_LNS_SUB:
        case TOKEN_INST_32IM_LNS_MUL:
        case TOKEN_INST_32IM_LNS_DIV:
        case TOKEN_INST_32IM_LNS_SQT:

        default: {
          error(
            FATAL,
            "mapper - unknown instruction type in ",
            __FUNCTION__,
            __FILE__,
            __LINE__
          );
        }
      }

      switch (optype) {
        case OPTYPE_R: {
          insts[s_insts++] = riscv_map_r_type(
            funct7,
            lexer::riscv_token_get_reg(inst->f3->type),
            lexer::riscv_token_get_reg(inst->f2->type),
            funct3,
            lexer::riscv_token_get_reg(inst->f1->type),
            opcode
          );
          break;
        }
        case OPTYPE_I: {
          insts[s_insts++] = riscv_map_i_type(
            inst->f3->lit.number,
            lexer::riscv_token_get_reg(inst->f2->type),
            funct3,
            lexer::riscv_token_get_reg(inst->f1->type),
            opcode
          );
          break;
        }
        case OPTYPE_S: {
          insts[s_insts++] = riscv_map_s_type(
            inst->f3->lit.number,
            lexer::riscv_token_get_reg(inst->f2->type),
            lexer::riscv_token_get_reg(inst->f1->type),
            funct3,
            opcode
          );
          break;
        }
        case OPTYPE_B: {
          insts[s_insts++] = riscv_map_b_type(
            inst->f3->lit.number,
            lexer::riscv_token_get_reg(inst->f2->type),
            lexer::riscv_token_get_reg(inst->f1->type),
            funct3,
            opcode
          );
          break;
        }
        case OPTYPE_U: {
          insts[s_insts++] = riscv_map_s_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            opcode
          );
          break;
        }
        case OPTYPE_J: {
          insts[s_insts++] = riscv_map_s_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            opcode
          );
          break;
        }
        default: {
          error(
            FATAL,
            "mapper - invalid optype in ",
            __FUNCTION__,
            __FILE__,
            __LINE__
          );
        }
      }
    }
  }

  uint32_t* map_data2bin(const RISCVAST* ast, uint64_t& s_data) {
    error(FATAL, ast == nullptr, "mapper - ast is a nullptr in ", __FUNCTION__, __FILE__, __LINE__);

    uint64_t total_s_data = 0;
    for (uint64_t i = 0; i < ast->s_data; i++) {
      if (ast->data[i].type->type != lexer::TOKEN_STRING) {
        total_s_data += lexer::riscv_token_get_type_size(ast->data[i].type->type) * ast->data[i].s_arr;
        continue;
      }

      for (uint64_t j = 0; j < ast->data[i].s_arr; j++)
        total_s_data += strlen(ast->data[i].arr[j]->lit.string) + 1;
    }

    s_data = (s_data >> 2) + (s_data & 0b11 > 0);
    uint32_t* data = (uint32_t*)malloc(s_data * sizeof(uint32_t));
    error(FATAL, data == nullptr, "mapper - allocation of data array returned a nullptr in ", __FUNCTION__, __FILE__, __LINE__);

    uint64_t k = 0;
    for (uint64_t i = 0; i < ast->s_data; i++) {
      const bool string = ast->data[i].type->type == lexer::TOKEN_STRING;

      if (string) {
        for (uint64_t j = 0; j < ast->data[i].s_arr; j++) {
          char* c = ast->data[i].arr[j]->lit.string;
          error(FATAL, c == nullptr, "mapper - literal string is a nullptr in a TOKEN_LIT_STRING", "", __FILE__, __LINE__);

          for (uint8_t l = 0; true; l = (l + 1) & 4, c++) {
            data[k] ^= (uint32_t)(*c) << (l << 3);
            k += l == 3;
            if (*c == '\0')
              break;
          }
        }

        continue;
      }

      const uint64_t 
        c = lexer::riscv_token_get_type_size(ast->data[i].type->type),
        s_word = lexer::riscv_token_get_type_size(lexer::TOKEN_WORD),
        s_half = lexer::riscv_token_get_type_size(lexer::TOKEN_HALF);

      for (uint64_t j = 0; j < ast->data[i].s_arr; j += s_word - c + 1) {
        uint32_t word = 0;

        if (c == s_word) {
          word ^= (uint32_t)ast->data[i].arr[j]->lit.number;
        } else if (c == s_half) {
          word ^= (uint32_t)ast->data[i].arr[j]->lit.number;
          word ^= j + 1 < ast->data[i].s_arr ? (uint32_t)ast->data[i].arr[j + 1]->lit.number << 16 : 0;
        } else {
          word ^= (uint32_t)ast->data[i].arr[j]->lit.number;
          word ^= j + 1 < ast->data[i].s_arr ? (uint32_t)ast->data[i].arr[j + 1]->lit.number << 8 : 0;
          word ^= j + 2 < ast->data[i].s_arr ? (uint32_t)ast->data[i].arr[j + 2]->lit.number << 16 : 0;
          word ^= j + 3 < ast->data[i].s_arr ? (uint32_t)ast->data[i].arr[j + 3]->lit.number << 24 : 0;
        }

        data[k++] |= word;
      }
    }

    return data;
  }

  void write(const char* filename, const uint32_t* array, const uint64_t s_array) {

  }
}

inline uint32_t riscv_map_r_type(
  const uint8_t funct7, const uint8_t rb, const uint8_t ra,
  const uint8_t funct3, const uint8_t rd, const uint8_t opcode
) {
  return (
    (funct7 << 25) ^
    ((0x1F & rb) << 20) ^
    ((0x1F & ra) << 15) ^
    ((0x7 & funct3) << 12) ^
    ((0x1F & rd) << 7) ^
    (0x7F & opcode)
  );
}

inline uint32_t riscv_map_i_type(
  const uint16_t imm, const uint8_t ra, const uint8_t funct3,
  const uint8_t rd, const uint8_t opcode
) {
  return (
    (imm << 20) ^
    ((0x1F & ra) << 15) ^
    ((0x7 & funct3) << 12) ^
    ((0x1F & rd) << 7) ^
    (0x7F & opcode)
  );
}

inline uint32_t riscv_map_s_type(
  const uint16_t imm, const uint8_t rb, const uint8_t ra,
  const uint8_t funct3, const uint8_t opcode
) {
  return (
    ((imm >> 5) << 25) ^
    ((ra & 0x1F) << 15) ^
    ((funct3 & 0x7) << 12) ^
    ((imm & 0x1F) << 7) ^
    (opcode & 0x7F)
  );
}

inline uint32_t riscv_map_b_type(
  const uint16_t imm, const uint8_t rb, const uint8_t ra,
  const uint8_t funct3, const uint8_t opcode
) {
  return (
    (((((imm >> 12) & 1) << 6) | (imm >> 5)) << 25) ^
    ((ra & 0x1F) << 15) ^
    ((funct3 & 0x7) << 12) ^
    ((((imm & 0x1F) < 1) & ((imm >> 11) & 1)) << 7) ^
    (opcode & 0x7F)
  );
}

inline uint32_t riscv_map_u_type(const uint32_t imm, const uint8_t rd, const uint8_t opcode) {
  return (
    (imm & 0xFFFFF000) ^
    ((rd & 0x1F) << 5) ^
    (opcode & 0x7F)
  );
}

inline uint32_t riscv_map_j_type(const uint32_t imm, const uint8_t rd, const uint8_t opcode) {
  return (
    (((imm & 0x00100000) ^ ((imm & 0x00007FE) << 12) ^ ((imm & 0x00008000) >> 2) ^ (imm & 0x00FF000)) << 12) ^
    ((rd & 0x1F) << 5) ^
    (opcode & 0x7F)
  );
}
