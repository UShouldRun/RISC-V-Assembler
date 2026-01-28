#include "mapper_private.hpp"

namespace mapper {
  uint32_t* map_inst2bin(const parser::RISCVAST* ast, uint64_t& s_insts, const uint64_t s_header, const uint64_t s_data) {
    error(FATAL, ast == nullptr, "mapper - ast is a nullptr in map_inst2bin", "", __FILE__, __LINE__);

    // using this instead of std::string in the unordered_map
    // in order to save memory
    auto cstr_hash = [](const char* str) { 
      std::size_t hash = 0;
      while (*str) {
        hash = hash * 31 + *str++;
      }
      return hash;
    };

    auto cstr_equal = [](const char* a, const char* b) { 
      return std::strcmp(a, b) == 0; 
    };

    std::unordered_map<const char*, uint32_t, decltype(cstr_hash), decltype(cstr_equal)> map(
      0,
      cstr_hash,
      cstr_equal
    );

    uint32_t data_cursor = 0;
    for (uint64_t i = 0; i < ast->s_data; i++) {
      map.insert({ ast->data[i].symbol->lit.string, data_cursor });

      if (ast->data[i].type->type != lexer::TOKEN_STRING) {
        data_cursor += lexer::riscv_token_get_type_size(ast->data[i].type->type) * ast->data[i].s_arr;
        continue;
      }

      for (uint64_t j = 0; j < ast->data[i].s_arr; j++)
        data_cursor += strlen(ast->data[i].arr[j]->lit.string) + 1;
    }

    // s_data is already align4 according to map_data2bin, as it is the number of words (4 bytes each)
    // it needs to store the data section
    const uint32_t text_base = s_header + (s_data << 2);

    uint32_t text_cursor = text_base;
    for (uint64_t i = 0; i < ast->s_text; i++) {
      const parser::RISCVASTN_Text* inst = &(ast->text[i]);
      const lexer::RISCVTokenType type = ast->text[i].inst->type;

      if (!lexer::riscv_token_is_symbol(type)) {
        text_cursor += 4;

        switch (type) {
          case lexer::TOKEN_INST_32IM_MOVE_LI: {
            text_cursor += (inst->f2->lit.number > 0x00000FFF) * 4; // lower bound for load immediate needs one more inst
            break;
          }
          case lexer::TOKEN_INST_32IM_LS_LB:
          case lexer::TOKEN_INST_32IM_LS_LH:
          case lexer::TOKEN_INST_32IM_LS_LW: {
            text_cursor += lexer::riscv_token_is_symbol(inst->f2->type) * 4;
            break;
          }
          case lexer::TOKEN_INST_32IM_MOVE_LA:
          case lexer::TOKEN_INST_32IM_FC_CALL: {
            text_cursor += 4;
            break;
          }
          default: { break; }
        }

        continue;
      }

      map.insert({ ast->text[i].inst->lit.string, text_cursor });
    }

    /*
     * used for debug
      for (const auto& [label, addr] : map)
        std::cout << "Label: " << label << " -> 0x" << std::hex << addr << std::endl;
     * */
 
    uint64_t max_s_insts = ast->s_text >= 4 ? ast->s_text : 4;
    uint32_t* insts = (uint32_t*)malloc(max_s_insts * sizeof(uint32_t));
    error(FATAL, insts == nullptr, "mapper - allocation of instruction array returned a nullptr", "", __FILE__, __LINE__);

    for (uint64_t i = 0; i < ast->s_text; i++) {
      if (s_insts + 1 >= max_s_insts) { // In case we get a pseudo instruction that needs 2 instructions
        max_s_insts += max_s_insts >> 2;
        insts = (uint32_t*)realloc(insts, max_s_insts * sizeof(uint32_t));
        error(FATAL, insts == nullptr, "mapper - reallocation of instruction array returned a nullptr", "", __FILE__, __LINE__);
      }
      const uint32_t pc = text_base + (s_insts << 2);

      const parser::RISCVASTN_Text* inst = &(ast->text[i]);

      if (lexer::riscv_token_is_symbol(inst->inst->type))
        continue;

      OpType optype = OPTYPE_NONE;
      uint8_t 
        opcode = 0x00,
        funct3 = 0x0,
        funct7 = 0x00;

      switch (inst->inst->type) {
        case lexer::TOKEN_INST_32IM_NOP: {
          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_ADDI
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_MOVE_LA: {
          const uint32_t target_addr = map[inst->f2->lit.string];
          const int32_t offset = (int32_t)riscv_map_relative_addr(pc, target_addr);
          
          /*
           * used for debug
          std::cout << "LA: pc=0x" << std::hex << pc 
                    << " target=0x" << target_addr 
                    << " offset=" << std::dec << offset << std::endl;
           * */
                    
          // Split with proper sign handling
          int32_t
            upper = offset & 0xFFFFF000,
            lower = offset & 0xFFF;
          
          // If lower will be sign-extended as negative by addi, compensate upper
          if (lower & 0x800)
            upper += 0x1000;
          
          insts[s_insts++] = riscv_map_u_type(
            upper,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_i_type(
            lower,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_ADDI
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_MOVE_LI: {
          insts[s_insts++] = riscv_map_i_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_ADDI
          );
          if (inst->f2->lit.number <= 0x00000FFF) // lower bound for load immediate
            continue;

          insts[s_insts++] = riscv_map_u_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_LUI
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_MOVE_LUI: {
          optype = OPTYPE_U;
          opcode = OPCODE_LUI;
          break;
        }

        case lexer::TOKEN_INST_32IM_MOVE_AUIPC: {
          optype = OPTYPE_U;
          opcode = OPCODE_AUIPC;
          break;
        }

        case lexer::TOKEN_INST_32IM_MOVE_MV: {
          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_ADDI
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_ALS_NEG: {
          insts[s_insts++] = riscv_map_r_type(
            0x20,
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_SUB
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_ALS_ADD: {
          optype = OPTYPE_R;
          opcode = OPCODE_ADD;
          funct3 = 0x0;
          funct7 = 0x00;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_ADDI: {
          optype = OPTYPE_I;
          opcode = OPCODE_ADDI;
          funct3 = 0x0;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_SUB: {
          optype = OPTYPE_R;
          opcode = OPCODE_SUB;
          funct3 = 0x0;
          funct7 = 0x20;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_NOT: {
          insts[s_insts++] = riscv_map_i_type(
            0xFFF,
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            0x4,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_XORI
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_ALS_AND: {
          optype = OPTYPE_R;
          opcode = OPCODE_AND;
          funct3 = FUNCT3_AND;
          funct7 = FUNCT7_AND;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_ANDI: {
          optype = OPTYPE_I;
          opcode = OPCODE_ANDI;
          funct3 = FUNCT3_ANDI;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_OR: {
          optype = OPTYPE_R;
          opcode = OPCODE_OR;
          funct3 = FUNCT3_OR;
          funct7 = FUNCT7_OR;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_ORI: {
          optype = OPTYPE_I;
          opcode = OPCODE_ORI;
          funct3 = FUNCT3_ORI;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_XOR: {
          optype = OPTYPE_R;
          opcode = OPCODE_XOR;
          funct3 = FUNCT3_XOR;
          funct7 = FUNCT7_XOR;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_XORI: {
          optype = OPTYPE_I;
          opcode = OPCODE_XORI;
          funct3 = FUNCT3_XORI;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_SLL: {
          optype = OPTYPE_R;
          opcode = OPCODE_SLL;
          funct3 = FUNCT3_SLL;
          funct7 = FUNCT7_SLL;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_SLLI: {
          optype = OPTYPE_I;
          opcode = OPCODE_SLLI;
          funct3 = FUNCT3_SLLI;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_SRL: {
          optype = OPTYPE_R;
          opcode = OPCODE_SRL;
          funct3 = FUNCT3_SRL;
          funct7 = FUNCT7_SRL;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_SRLI: {
          optype = OPTYPE_I;
          opcode = OPCODE_SRLI;
          funct3 = FUNCT3_SRLI;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_SRA: {
          optype = OPTYPE_R;
          opcode = OPCODE_SRA;
          funct3 = FUNCT3_SRA;
          funct7 = FUNCT7_SRA;
          break;
        }

        case lexer::TOKEN_INST_32IM_ALS_SRAI: {
          optype = OPTYPE_I;
          opcode = OPCODE_SRAI;
          funct3 = FUNCT3_SRAI;
          break;
        }

        case lexer::TOKEN_INST_32IM_MD_MUL: {
          optype = OPTYPE_R;
          opcode = OPCODE_MUL;
          funct3 = FUNCT3_MUL;
          funct7 = FUNCT7_MUL;
          break;
        }

        case lexer::TOKEN_INST_32IM_MD_MULH: {
          optype = OPTYPE_R;
          opcode = OPCODE_MULH;
          funct3 = FUNCT3_MULH;
          funct7 = FUNCT7_MULH;
          break;
        }

        case lexer::TOKEN_INST_32IM_MD_MULSU: {
          optype = OPTYPE_R;
          opcode = OPCODE_MULSU;
          funct3 = FUNCT3_MULSU;
          funct7 = FUNCT7_MULSU;
          break;
        }

        case lexer::TOKEN_INST_32IM_MD_MULU: {
          optype = OPTYPE_R;
          opcode = OPCODE_MULU;
          funct3 = FUNCT3_MULU;
          funct7 = FUNCT7_MULU;
          break;
        }

        case lexer::TOKEN_INST_32IM_MD_DIV: {
          optype = OPTYPE_R;
          opcode = OPCODE_DIV;
          funct3 = FUNCT3_DIV;
          funct7 = FUNCT7_DIV;
          break;
        }

        case lexer::TOKEN_INST_32IM_MD_DIVU: {
          optype = OPTYPE_R;
          opcode = OPCODE_DIVU;
          funct3 = FUNCT3_DIVU;
          funct7 = FUNCT7_DIVU;
          break;
        }

        case lexer::TOKEN_INST_32IM_MD_REM: {
          optype = OPTYPE_R;
          opcode = OPCODE_REM;
          funct3 = FUNCT3_REM;
          funct7 = FUNCT7_REM;
          break;
        }

        case lexer::TOKEN_INST_32IM_MD_REMU: {
          optype = OPTYPE_R;
          opcode = OPCODE_REMU;
          funct3 = FUNCT3_REMU;
          funct7 = FUNCT7_REMU;
          break;
        }

        case lexer::TOKEN_INST_32IM_LS_LB: {
          optype = OPTYPE_I;
          opcode = OPCODE_LB;
          funct3 = FUNCT3_LB;
          if (!lexer::riscv_token_is_symbol(inst->f2->type))
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          insts[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_i_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_LB,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_LB
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_LS_LH: {
          optype = OPTYPE_I;
          opcode = OPCODE_LH;
          funct3 = FUNCT3_LH;
          if (!lexer::riscv_token_is_symbol(inst->f2->type))
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          insts[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_i_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_LH,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_LH
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_LS_LW: {
          optype = OPTYPE_I;
          opcode = OPCODE_LW;
          funct3 = FUNCT3_LW;
          if (!lexer::riscv_token_is_symbol(inst->f2->type))
            break;
 
          const uint32_t addr = map[inst->f2->lit.string];
          insts[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_i_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_LW,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_LW
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_LS_LBU: {
          optype = OPTYPE_I;
          opcode = OPCODE_LBU;
          funct3 = FUNCT3_LBU;
          break;
        }

        case lexer::TOKEN_INST_32IM_LS_LHU: {
          optype = OPTYPE_I;
          opcode = OPCODE_LHU;
          funct3 = FUNCT3_LHU;
          break;
        }

        case lexer::TOKEN_INST_32IM_LS_SB: {
          optype = OPTYPE_S;
          opcode = OPCODE_SB;
          funct3 = FUNCT3_SB;
          if (!lexer::riscv_token_is_symbol(inst->f2->type))
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          insts[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_s_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_SB,
            OPCODE_SB
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_LS_SH: {
          optype = OPTYPE_S;
          opcode = OPCODE_SH;
          funct3 = FUNCT3_SH;
          if (!lexer::riscv_token_is_symbol(inst->f2->type))
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          insts[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_s_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_SH,
            OPCODE_SH
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_LS_SW: {
          optype = OPTYPE_S;
          opcode = OPCODE_SW;
          funct3 = FUNCT3_SW;
          if (!lexer::riscv_token_is_symbol(inst->f2->type))
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          insts[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_s_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_SW,
            OPCODE_SW
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_CP_SLT: {
          optype = OPTYPE_R;
          opcode = OPCODE_SLT;
          funct3 = FUNCT3_SLT;
          funct7 = FUNCT7_SLT;
          break;
        }

        case lexer::TOKEN_INST_32IM_CP_SLTI: {
          optype = OPTYPE_I;
          opcode = OPCODE_SLTI;
          funct3 = FUNCT3_SLTI;
          break;
        }

        case lexer::TOKEN_INST_32IM_CP_SLTU: {
          optype = OPTYPE_R;
          opcode = OPCODE_SLTU;
          funct3 = FUNCT3_SLTU;
          funct7 = FUNCT7_SLTU;
          break;
        }

        case lexer::TOKEN_INST_32IM_CP_SLTIU: {
          optype = OPTYPE_I;
          opcode = OPCODE_SLTIU;
          funct3 = FUNCT3_SLTIU;
          break;
        }

        case lexer::TOKEN_INST_32IM_CP_SEQZ: {
          insts[s_insts++] = riscv_map_i_type(
            1,
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_SLTIU,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_SLTIU
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_CP_SNEZ: {
          insts[s_insts++] = riscv_map_r_type(
            FUNCT7_SLTU,
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_SLTU,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_SLTU
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_CP_SLTZ: {
          insts[s_insts++] = riscv_map_r_type(
            FUNCT7_SLT,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_SLT,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_SLT
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_CP_SGTZ: {
          insts[s_insts++] = riscv_map_r_type(
            FUNCT7_SLT,
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_SLT,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_SLT
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BEQ: {
          optype = OPTYPE_B;
          opcode = OPCODE_BEQ;
          funct3 = FUNCT3_BEQ;
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_BNE: {
          optype = OPTYPE_B;
          opcode = OPCODE_BNE;
          funct3 = FUNCT3_BNE;
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_BGT: {
          const uint32_t offset = lexer::riscv_token_is_symbol(inst->f3->type) 
            ? riscv_map_relative_addr(pc, map[inst->f3->lit.string])
            : inst->f3->lit.number;

          insts[s_insts++] = riscv_map_b_type(
            offset,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BLT,
            OPCODE_BLT
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BGE: {
          optype = OPTYPE_B;
          opcode = OPCODE_BGE;
          funct3 = FUNCT3_BGE;
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_BLE: {
          const uint32_t offset = lexer::riscv_token_is_symbol(inst->f3->type) 
            ? riscv_map_relative_addr(pc, map[inst->f3->lit.string])
            : inst->f3->lit.number;

          insts[s_insts++] = riscv_map_b_type(
            offset,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BGE,
            OPCODE_BGE
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BLT: {
          optype = OPTYPE_B;
          opcode = OPCODE_BLT;
          funct3 = FUNCT3_BLT;
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_BGTU: {
          const uint32_t offset = lexer::riscv_token_is_symbol(inst->f3->type) 
            ? riscv_map_relative_addr(pc, map[inst->f3->lit.string])
            : inst->f3->lit.number;

          insts[s_insts++] = riscv_map_b_type(
            offset,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BLTU,
            OPCODE_BLTU
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BGEU: {
          optype = OPTYPE_B;
          opcode = OPCODE_BGEU;
          funct3 = FUNCT3_BGEU;
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_BLTU: {
          optype = OPTYPE_B;
          opcode = OPCODE_BLTU;
          funct3 = FUNCT3_BLTU;
          break;
        }

        case lexer::TOKEN_INST_32IM_FC_BLEU: {
          const uint32_t offset = lexer::riscv_token_is_symbol(inst->f3->type) 
            ? riscv_map_relative_addr(pc, map[inst->f3->lit.string])
            : inst->f3->lit.number;

          insts[s_insts++] = riscv_map_b_type(
            offset,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BGEU,
            OPCODE_BGEU
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BEQZ: {
          insts[s_insts++] = riscv_map_b_type(
            riscv_map_relative_addr(pc, map[inst->f2->lit.string]),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BEQ,
            OPCODE_BEQ
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BNEZ: {
          insts[s_insts++] = riscv_map_b_type(
            riscv_map_relative_addr(pc, map[inst->f2->lit.string]),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BNE,
            OPCODE_BNE
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BLEZ: {
          insts[s_insts++] = riscv_map_b_type(
            riscv_map_relative_addr(pc, map[inst->f2->lit.string]),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BGE,
            OPCODE_BGE
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BGEZ: {
          insts[s_insts++] = riscv_map_b_type(
            riscv_map_relative_addr(pc, map[inst->f2->lit.string]),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BGE,
            OPCODE_BGE
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BLTZ: {
          insts[s_insts++] = riscv_map_b_type(
            riscv_map_relative_addr(pc, map[inst->f2->lit.string]),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BLT,
            OPCODE_BLT
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_BGTZ: {
          insts[s_insts++] = riscv_map_b_type(
            riscv_map_relative_addr(pc, map[inst->f2->lit.string]),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_BLT,
            OPCODE_BLT
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_J: {
          insts[s_insts++] = riscv_map_j_type(
            riscv_map_relative_addr(pc, map[inst->f1->lit.string]),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_JAL
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_JAL: {
          optype = OPTYPE_J;
          opcode = OPCODE_JAL;
          if (inst->f2 != nullptr)
            break;

          insts[s_insts++] = riscv_map_j_type(
            riscv_map_relative_addr(pc, map[inst->f1->lit.string]),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_JAL
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_JR: {
          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_JALR,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_JALR
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_JALR: {
          optype = OPTYPE_I;
          opcode = OPCODE_JALR;
          if (inst->f2 != nullptr)
            break;

          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_JALR,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_JALR
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_CALL: {
          insts[s_insts++] = riscv_map_u_type(
            inst->f1->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_i_type(
            inst->f1->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_JALR,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_JALR
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_FC_RET: {
          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_JALR,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_JALR
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_OS_ECALL: {
          insts[s_insts++] = riscv_map_i_type(
            IMM_ECALL,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_OS
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_OS_EBREAK: {
          insts[s_insts++] = riscv_map_i_type(
            IMM_EBREAK,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_OS
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_OS_SRET: {
          insts[s_insts++] = riscv_map_i_type(
            IMM_SRET,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_OS
          );
          continue;
        }

        case lexer::TOKEN_INST_32IM_LNS_ADD: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_ADD;
          funct3 = FUNCT3_LNS_ADD;
          funct7 = FUNCT7_LNS_ADD;
          break;
        }

        case lexer::TOKEN_INST_32IM_LNS_SUB: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_SUB;
          funct3 = FUNCT3_LNS_SUB;
          funct7 = FUNCT7_LNS_SUB;
          break;
        }

        case lexer::TOKEN_INST_32IM_LNS_MUL: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_MUL;
          funct3 = FUNCT3_LNS_MUL;
          funct7 = FUNCT7_LNS_MUL;
          break;
        }

        case lexer::TOKEN_INST_32IM_LNS_DIV: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_DIV;
          funct3 = FUNCT3_LNS_DIV;
          funct7 = FUNCT7_LNS_DIV;
          break;
        }

        case lexer::TOKEN_INST_32IM_LNS_SQT: {
          insts[s_insts++] = riscv_map_r_type(
            FUNCT7_LNS_SQT,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            FUNCT3_LNS_SQT,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            OPCODE_LNS_SQT
          );
          continue;
        }

        default: {
          error(
            FATAL,
            true,
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
            lexer::riscv_token_get_reg(inst->f3->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            funct3,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            opcode
          );
          break;
        }
        case OPTYPE_I: {
          const bool                  load_or_jalr = (
            lexer::riscv_token_is_inst_load(inst->inst->type) ||
            inst->inst->type == lexer::TOKEN_INST_32IM_FC_JALR
          );

          const int32_t               imm = load_or_jalr ? inst->f2->lit.number : inst->f3->lit.number;
          const lexer::RISCVTokenType rs1 = load_or_jalr ? inst->f3->type       : inst->f2->type;

          insts[s_insts++] = riscv_map_i_type(
            imm,
            lexer::riscv_token_get_reg(rs1, __FUNCTION__, __FILE__, __LINE__),
            funct3,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            opcode
          );
          break;
        }
        case OPTYPE_S: {
          insts[s_insts++] = riscv_map_s_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f3->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            funct3,
            opcode
          );
          break;
        }
        case OPTYPE_B: {
          const uint32_t offset = lexer::riscv_token_is_symbol(inst->f3->type) 
            ? riscv_map_relative_addr(pc, map[inst->f3->lit.string])
            : inst->f3->lit.number;

          insts[s_insts++] = riscv_map_b_type(
            offset,
            lexer::riscv_token_get_reg(inst->f2->type, __FUNCTION__, __FILE__, __LINE__),
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            funct3,
            opcode
          );
          break;
        }
        case OPTYPE_U: {
          insts[s_insts++] = riscv_map_u_type(
            inst->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            opcode
          );
          break;
        }
        case OPTYPE_J: {
          const uint32_t offset = lexer::riscv_token_is_symbol(inst->f2->type) 
            ? riscv_map_relative_addr(pc, map[inst->f2->lit.string])
            : inst->f2->lit.number;

          insts[s_insts++] = riscv_map_j_type(
            offset,
            lexer::riscv_token_get_reg(inst->f1->type, __FUNCTION__, __FILE__, __LINE__),
            opcode
          );
          break;
        }
        default: {
          error(
            FATAL,
            true,
            "mapper - invalid optype in ",
            __FUNCTION__,
            __FILE__,
            __LINE__
          );
        }
      }
    }

    return insts;
  }

  uint32_t* map_data2bin(const parser::RISCVAST* ast, uint64_t& s_data) {
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

    s_data = (total_s_data >> 2) + ((total_s_data & 0b11) > 0); // number of words + padding to allocate
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
        c      = lexer::riscv_token_get_type_size(ast->data[i].type->type),
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

  void write(const char* filename, const RISCVEncoding& encoding) {
    const uint64_t len_filename = strlen(filename);
    error(
      FATAL,
      len_filename < 2,
      "mapper - filename as length less than 2 (it should end in \".s\"): ",
      filename,
      __FILE__,
      __LINE__
    );

    char* output_filename = (char*)malloc((len_filename + 3) * sizeof(char));
    error(FATAL, output_filename == nullptr, "mapper - could not allocate memory for output filename", "", __FILE__, __LINE__);
    strncpy(output_filename, filename, len_filename + 3);
    output_filename[len_filename - 2] = '.';
    output_filename[len_filename - 1] = 'b';
    output_filename[len_filename] = 'i';
    output_filename[len_filename + 1] = 'n';
    output_filename[len_filename + 2] = '\0';

    std::ofstream file(output_filename, std::ios::binary);
    error(FATAL, !file.is_open(), "mapper - could not open output file ", filename, __FILE__, __LINE__);
    log("mapper - opened output file ", filename, __FILE__, __LINE__);

    file.write(reinterpret_cast<const char*>(&encoding.s_data),  sizeof(uint64_t));
    file.write(reinterpret_cast<const char*>(&encoding.s_insts), sizeof(uint64_t));

    for (uint64_t i = 0; i < encoding.s_data; i++)
      file.write(reinterpret_cast<const char*>(&(encoding.data[i])), sizeof(uint32_t));
    for (uint64_t i = 0; i < encoding.s_insts; i++)
      file.write(reinterpret_cast<const char*>(&(encoding.insts[i])), sizeof(uint32_t));

    log("mapper - instructions written to the output file", filename, __FILE__, __LINE__);
    free(output_filename);
  }
}

inline uint32_t riscv_map_r_type(
  const uint8_t funct7, const uint8_t rb, const uint8_t ra,
  const uint8_t funct3, const uint8_t rd, const uint8_t opcode
) {
  // R-type: funct7[31:25], rs2[24:20], rs1[19:15], funct3[14:12], rd[11:7], opcode[6:0]
  return (
    ((uint32_t)funct7 << 25) |
    ((uint32_t)(rb & 0x1F) << 20) |
    ((uint32_t)(ra & 0x1F) << 15) |
    ((uint32_t)(funct3 & 0x7) << 12) |
    ((uint32_t)(rd & 0x1F) << 7) |
    ((uint32_t)(opcode & 0x7F))
  );
}

inline uint32_t riscv_map_i_type(
  const uint16_t imm, const uint8_t ra, const uint8_t funct3,
  const uint8_t rd, const uint8_t opcode
) {
  // I-type: imm[31:20], rs1[19:15], funct3[14:12], rd[11:7], opcode[6:0]
  return (
    ((uint32_t)(imm & 0xFFF) << 20) |
    ((uint32_t)(ra & 0x1F) << 15) |
    ((uint32_t)(funct3 & 0x7) << 12) |
    ((uint32_t)(rd & 0x1F) << 7) |
    ((uint32_t)(opcode & 0x7F))
  );
}

inline uint32_t riscv_map_s_type(
  const uint16_t imm, const uint8_t rb, const uint8_t ra,
  const uint8_t funct3, const uint8_t opcode
) {
  // S-type: imm[11:5] -> [31:25], rs2[24:20], rs1[19:15], funct3[14:12], imm[4:0] -> [11:7], opcode[6:0]
  const uint32_t imm11_5 = (imm >> 5) & 0x7F;
  const uint32_t imm4_0  = imm & 0x1F;

  return (
    (imm11_5 << 25) |
    ((uint32_t)(rb & 0x1F) << 20) |
    ((uint32_t)(ra & 0x1F) << 15) |
    ((uint32_t)(funct3 & 0x7) << 12) |
    (imm4_0 << 7) |
    ((uint32_t)(opcode & 0x7F))
  );
}

inline uint32_t riscv_map_b_type(
  const uint16_t imm, const uint8_t rb, const uint8_t ra,
  const uint8_t funct3, const uint8_t opcode
) {
  // B-type: imm[12] -> 31, imm[10:5] -> 30:25, rs2 -> 24:20, rs1 -> 19:15, funct3 -> 14:12, imm[4:1] -> 11:8, imm[11] -> 7, opcode -> 6:0
  const uint32_t imm12   = (imm >> 12) & 0x1;
  const uint32_t imm10_5 = (imm >> 5)  & 0x3F;
  const uint32_t imm4_1  = (imm >> 1)  & 0xF;
  const uint32_t imm11   = (imm >> 11) & 0x1;

  return (
    (imm12   << 31) |
    (imm10_5 << 25) |
    ((uint32_t)(rb & 0x1F) << 20) |
    ((uint32_t)(ra & 0x1F) << 15) |
    ((uint32_t)(funct3 & 0x7) << 12) |
    (imm4_1  << 8) |
    (imm11   << 7) |
    ((uint32_t)(opcode & 0x7F))
  );
}

inline uint32_t riscv_map_u_type(const uint32_t imm, const uint8_t rd, const uint8_t opcode) {
  // U-type: imm[31:12], rd[11:7], opcode[6:0]
  return (
    (imm & 0xFFFFF000) |
    ((uint32_t)(rd & 0x1F) << 7) |
    ((uint32_t)(opcode & 0x7F))
  );
}

inline uint32_t riscv_map_j_type(const uint32_t imm, const uint8_t rd, const uint8_t opcode) {
  // J-type: imm[20] -> 31, imm[10:1] -> 30:21, imm[11] -> 20, imm[19:12] -> 19:12, rd -> 11:7, opcode -> 6:0
  const uint32_t imm20    = (imm >> 20) & 0x1;
  const uint32_t imm10_1  = (imm >> 1)  & 0x3FF;
  const uint32_t imm11    = (imm >> 11) & 0x1;
  const uint32_t imm19_12 = (imm >> 12) & 0xFF;

  return (
    (imm20    << 31) |
    (imm10_1  << 21) |
    (imm11    << 20) |
    (imm19_12 << 12) |
    ((uint32_t)(rd & 0x1F) << 7) |
    ((uint32_t)(opcode & 0x7F))
  );
}

inline uint32_t riscv_map_relative_addr(const uint32_t pc, const uint32_t addr) {
  // addresses should be multiples of 4 already
  return static_cast<uint32_t>(((int32_t)addr - (int32_t)pc));
}
