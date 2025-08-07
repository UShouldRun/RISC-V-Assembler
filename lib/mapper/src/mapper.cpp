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
          const uint32_t addr = map[inst->f2->lit.string];
          insts[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_i_type(
            addr,
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
          optype = OPTYPE_R;
          opcode = OPCODE_AND;
          funct3 = FUNCT3_AND;
          funct7 = FUNCT7_AND;
          break;
        }

        case TOKEN_INST_32IM_ALS_ANDI: {
          optype = OPTYPE_I;
          opcode = OPCODE_ANDI;
          funct3 = FUNCT3_ANDI;
          break;
        }

        case TOKEN_INST_32IM_ALS_OR: {
          optype = OPTYPE_R;
          opcode = OPCODE_OR;
          funct3 = FUNCT3_OR;
          funct7 = FUNCT7_OR;
          break;
        }

        case TOKEN_INST_32IM_ALS_ORI: {
          optype = OPTYPE_I;
          opcode = OPCODE_ORI;
          funct3 = FUNCT3_ORI;
          break;
        }

        case TOKEN_INST_32IM_ALS_XOR: {
          optype = OPTYPE_R;
          opcode = OPCODE_XOR;
          funct3 = FUNCT3_XOR;
          funct7 = FUNCT7_XOR;
          break;
        }

        case TOKEN_INST_32IM_ALS_XORI: {
          optype = OPTYPE_I;
          opcode = OPCODE_XORI;
          funct3 = FUNCT3_XORI;
          break;
        }

        case TOKEN_INST_32IM_ALS_SLL: {
          optype = OPTYPE_R;
          opcode = OPCODE_SLL;
          funct3 = FUNCT3_SLL;
          funct7 = FUNCT7_SLL;
          break;
        }

        case TOKEN_INST_32IM_ALS_SLLI: {
          optype = OPTYPE_I;
          opcode = OPCODE_SLLI;
          funct3 = FUNCT3_SLLI;
          break;
        }

        case TOKEN_INST_32IM_ALS_SRL: {
          optype = OPTYPE_R;
          opcode = OPCODE_SRL;
          funct3 = FUNCT3_SRL;
          funct7 = FUNCT7_SRL;
          break;
        }

        case TOKEN_INST_32IM_ALS_SRLI: {
          optype = OPTYPE_I;
          opcode = OPCODE_SRLI;
          funct3 = FUNCT3_SRLI;
          break;
        }

        case TOKEN_INST_32IM_ALS_SRA: {
          optype = OPTYPE_R;
          opcode = OPCODE_SRA;
          funct3 = FUNCT3_SRA;
          funct7 = FUNCT7_SRA;
          break;
        }

        case TOKEN_INST_32IM_ALS_SRAI: {
          optype = OPTYPE_I;
          opcode = OPCODE_SRAI;
          funct3 = FUNCT3_SRAI;
          break;
        }

        case TOKEN_INST_32IM_MD_MUL: {
          optype = OPTYPE_R;
          opcode = OPCODE_MUL;
          funct3 = FUNCT3_MUL;
          funct7 = FUNCT7_MUL;
          break;
        }

        case TOKEN_INST_32IM_MD_MULH: {
          optype = OPTYPE_R;
          opcode = OPCODE_MULH;
          funct3 = FUNCT3_MULH;
          funct7 = FUNCT7_MULH;
          break;
        }

        case TOKEN_INST_32IM_MD_MULSU: {
          optype = OPTYPE_R;
          opcode = OPCODE_MULSU;
          funct3 = FUNCT3_MULSU;
          funct7 = FUNCT7_MULSU;
          break;
        }

        case TOKEN_INST_32IM_MD_MULU: {
          optype = OPTYPE_R;
          opcode = OPCODE_MULU;
          funct3 = FUNCT3_MULU;
          funct7 = FUNCT7_MULU;
          break;
        }

        case TOKEN_INST_32IM_MD_DIV: {
          optype = OPTYPE_R;
          opcode = OPCODE_DIV;
          funct3 = FUNCT3_DIV;
          funct7 = FUNCT7_DIV;
          break;
        }

        case TOKEN_INST_32IM_MD_DIVU: {
          optype = OPTYPE_R;
          opcode = OPCODE_DIVU;
          funct3 = FUNCT3_DIVU;
          funct7 = FUNCT7_DIVU;
          break;
        }

        case TOKEN_INST_32IM_MD_REM: {
          optype = OPTYPE_R;
          opcode = OPCODE_REM;
          funct3 = FUNCT3_REM;
          funct7 = FUNCT7_REM;
          break;
        }

        case TOKEN_INST_32IM_MD_REMU: {
          optype = OPTYPE_R;
          opcode = OPCODE_REMU;
          funct3 = FUNCT3_REMU;
          funct7 = FUNCT7_REMU;
          break;
        }

        case TOKEN_INST_32IM_LS_LB: {
          optype = OPTYPE_I;
          opcode = OPCODE_LB;
          funct3 = FUNCT3_LB;
          if (inst->f2 != lexer::TOKEN_SYMBOL)
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          inst[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_AUIPC
          );
          inst[s_insts++] = riscv_map_i_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_LB,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_LB
          );
          continue;
        }

        case TOKEN_INST_32IM_LS_LH: {
          optype = OPTYPE_I;
          opcode = OPCODE_LH;
          funct3 = FUNCT3_LH;
          if (inst->f2 != lexer::TOKEN_SYMBOL)
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          inst[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_AUIPC
          );
          inst[s_insts++] = riscv_map_i_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_LH,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_LH
          );
          continue;
        }

        case TOKEN_INST_32IM_LS_LW: {
          optype = OPTYPE_I;
          opcode = OPCODE_LW;
          funct3 = FUNCT3_LW;
          if (inst->f2 != lexer::TOKEN_SYMBOL)
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          inst[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_AUIPC
          );
          inst[s_insts++] = riscv_map_i_type(
            addr,
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_LW,
            lexer::riscv_token_get_reg(inst->f1->type),
            OPCODE_LW
          );
          continue;
        }

        case TOKEN_INST_32IM_LS_LBU: {
          optype = OPTYPE_I;
          opcode = OPCODE_LBU;
          funct3 = FUNCT3_LBU;
          break;
        }

        case TOKEN_INST_32IM_LS_LHU: {
          optype = OPTYPE_I;
          opcode = OPCODE_LHU;
          funct3 = FUNCT3_LHU;
          break;
        }

        case TOKEN_INST_32IM_LS_SB: {
          optype = OPTYPE_S;
          opcode = OPCODE_SB;
          funct3 = FUNCT3_SB;
          if (inst->f2 != lexer::TOKEN_SYMBOL)
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          inst[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type),
            OPCODE_AUIPC
          );
          inst[s_insts++] = riscv_map_s_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type),
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_SB,
            OPCODE_SB
          );
          continue;
        }

        case TOKEN_INST_32IM_LS_SH: {
          optype = OPTYPE_S;
          opcode = OPCODE_SH;
          funct3 = FUNCT3_SH;
          if (inst->f2 != lexer::TOKEN_SYMBOL)
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          inst[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type),
            OPCODE_AUIPC
          );
          inst[s_insts++] = riscv_map_s_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type),
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_SH,
            OPCODE_SH
          );
          continue;
        }

        case TOKEN_INST_32IM_LS_SW: {
          optype = OPTYPE_S;
          opcode = OPCODE_SW;
          funct3 = FUNCT3_SW;
          if (inst->f2 != lexer::TOKEN_SYMBOL)
            break;

          const uint32_t addr = map[inst->f2->lit.string];
          inst[s_insts++] = riscv_map_u_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type),
            OPCODE_AUIPC
          );
          inst[s_insts++] = riscv_map_s_type(
            addr,
            lexer::riscv_token_get_reg(inst->f3->type),
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_SW,
            OPCODE_SW
          );
          continue;
        }

        case TOKEN_INST_32IM_CP_SLT: {
          optype = OPTYPE_R;
          opcode = OPCODE_SLT;
          funct3 = FUNCT3_SLT;
          funct7 = FUNCT7_SLT;
          break;
        }

        case TOKEN_INST_32IM_CP_SLTI: {
          optype = OPTYPE_I;
          opcode = OPCODE_SLTI;
          funct3 = FUNCT3_SLTI;
          break;
        }

        case TOKEN_INST_32IM_CP_SLTU: {
          optype = OPTYPE_R;
          opcode = OPCODE_SLTU;
          funct3 = FUNCT3_SLTU;
          funct7 = FUNCT7_SLTU;
          break;
        }

        case TOKEN_INST_32IM_CP_SLTIU: {
          optype = OPTYPE_I;
          opcode = OPCODE_SLTIU;
          funct3 = FUNCT3_SLTIU;
          break;
        }

        case TOKEN_INST_32IM_CP_SEQZ: {
          insts[s_insts++] = riscv_map_i_type(
            1,
            lexer::riscv_token_get_reg(insts->f2->type),
            FUNCT3_SLTIU,
            lexer::riscv_token_get_reg(insts->f1->type),
            OPCODE_SLTIU
          );
          continue;
        }

        case TOKEN_INST_32IM_CP_SNEZ: {
          insts[s_insts++] = riscv_map_r_type(
            FUNCT7_SLTU,
            lexer::riscv_token_get_reg(insts->f2->type),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            FUNCT3_SLTU,
            lexer::riscv_token_get_reg(insts->f1->type),
            OPCODE_SLTU
          );
          continue;
        }

        case TOKEN_INST_32IM_CP_SLTZ: {
          insts[s_insts++] = riscv_map_r_type(
            FUNCT7_SLT,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            lexer::riscv_token_get_reg(insts->f2->type),
            FUNCT3_SLT,
            lexer::riscv_token_get_reg(insts->f1->type),
            OPCODE_SLT
          );
          continue;
        }

        case TOKEN_INST_32IM_CP_SGTZ: {
          insts[s_insts++] = riscv_map_r_type(
            FUNCT7_SLT,
            lexer::riscv_token_get_reg(insts->f2->type),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            FUNCT3_SLT,
            lexer::riscv_token_get_reg(insts->f1->type),
            OPCODE_SLT
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BEQ: {
          optype = OPTYPE_B;
          opcode = OPCODE_BEQ;
          funct3 = FUNCT3_BEQ;
          break;
        }

        case TOKEN_INST_32IM_FC_BNE: {
          optype = OPTYPE_B;
          opcode = OPCODE_BNE;
          funct3 = FUNCT3_BNE;
          break;
        }

        case TOKEN_INST_32IM_FC_BGT: {
          insts[s_insts++] = riscv_map_b_type(
            inst->f3->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            lexer::riscv_token_get_reg(inst->f2->type),
            FUNCT3_BLT,
            OPCODE_BLT
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BGE: {
          optype = OPTYPE_B;
          opcode = OPCODE_BGE;
          funct3 = FUNCT3_BGE;
          break;
        }

        case TOKEN_INST_32IM_FC_BLE: {
          insts[s_insts++] = riscv_map_b_type(
            inst->f3->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            lexer::riscv_token_get_reg(inst->f2->type),
            FUNCT3_BGE,
            OPCODE_BGE
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BLT: {
          optype = OPTYPE_B;
          opcode = OPCODE_BLT;
          funct3 = FUNCT3_BLT;
          break;
        }

        case TOKEN_INST_32IM_FC_BGTU: {
          insts[s_insts++] = riscv_map_b_type(
            inst->f3->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            lexer::riscv_token_get_reg(inst->f2->type),
            FUNCT3_BLTU,
            OPCODE_BLTU
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BGEU: {
          optype = OPTYPE_B;
          opcode = OPCODE_BGEU;
          funct3 = FUNCT3_BGEU;
          break;
        }

        case TOKEN_INST_32IM_FC_BLTU: {
          optype = OPTYPE_B;
          opcode = OPCODE_BLTU;
          funct3 = FUNCT3_BLTU;
          break;
        }

        case TOKEN_INST_32IM_FC_BLEU: {
          insts[s_insts++] = riscv_map_b_type(
            insts->f3->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            lexer::riscv_token_get_reg(inst->f2->type),
            FUNCT3_BGEU,
            OPCODE_BGEU
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BEQZ: {
          insts[s_insts++] = riscv_map_b_type(
            insts->f2->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_BEQ,
            OPCODE_BEQ
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BNEZ: {
          insts[s_insts++] = riscv_map_b_type(
            insts->f2->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_BNE,
            OPCODE_BNE
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BLEZ: {
          insts[s_insts++] = riscv_map_b_type(
            insts->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            FUNCT3_BGE,
            OPCODE_BGE
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BGEZ: {
          insts[s_insts++] = riscv_map_b_type(
            insts->f2->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_BGE,
            OPCODE_BGE
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BLTZ: {
          insts[s_insts++] = riscv_map_b_type(
            insts->f2->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_BLT,
            OPCODE_BLT
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_BGTZ: {
          insts[s_insts++] = riscv_map_b_type(
            insts->f2->lit.number,
            lexer::riscv_token_get_reg(inst->f1->type),
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            FUNCT3_BLT,
            OPCODE_BLT
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_J: {
          insts[s_insts++] = riscv_map_j_type(
            insts->f1->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            OPCODE_JAL
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_JAL: {
          optype = OPTYPE_J;
          opcode = OPCODE_JAL;
          if (inst->f2 != nullptr)
            break;

          insts[s_insts++] = riscv_map_j_type(
            insts->f1->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1),
            OPCODE_JAL
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_JR: {
          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_JALR,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            OPCODE_JALR
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_JALR: {
          optype = OPTYPE_I;
          opcode = OPCODE_JALR;
          if (inst->f2 != nullptr)
            break;

          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(inst->f1->type),
            FUNCT3_JALR,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1),
            OPCODE_JALR
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_CALL: {
          insts[s_insts++] = riscv_map_i_type(
            inst->f1->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1),
            OPCODE_AUIPC
          );
          insts[s_insts++] = riscv_map_i_type(
            inst->f1->lit.number,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1),
            FUNCT3_JALR,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            OPCODE_JALR
          );
          continue;
        }

        case TOKEN_INST_32IM_FC_RET: {
          insts[s_insts++] = riscv_map_i_type(
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X1),
            FUNCT3_JALR,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            OPCODE_JALR
          );
          continue;
        }

        case TOKEN_INST_32IM_OS_ECALL: {
          insts[s_insts++] = riscv_map_i_type(
            IMM_ECALL,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            OPCODE_OS
          );
          continue;
        }

        case TOKEN_INST_32IM_OS_EBREAK: {
          insts[s_insts++] = riscv_map_i_type(
            IMM_EBREAK,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            OPCODE_OS
          );
          continue;
        }

        case TOKEN_INST_32IM_OS_SRET: {
          insts[s_insts++] = riscv_map_i_type(
            IMM_SRET,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            0x0,
            lexer::riscv_token_get_reg(lexer::TOKEN_REG_X0),
            OPCODE_OS
          );
          continue;
        }

        case TOKEN_INST_32IM_LNS_ADD: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_ADD;
          funct3 = FUNCT3_LNS_ADD;
          funct7 = FUNCT7_LNS_ADD;
          break;
        }

        case TOKEN_INST_32IM_LNS_SUB: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_SUB;
          funct3 = FUNCT3_LNS_SUB;
          funct7 = FUNCT7_LNS_SUB;
          break;
        }

        case TOKEN_INST_32IM_LNS_MUL: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_MUL;
          funct3 = FUNCT3_LNS_MUL;
          funct7 = FUNCT7_LNS_MUL;
          break;
        }

        case TOKEN_INST_32IM_LNS_DIV: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_DIV;
          funct3 = FUNCT3_LNS_DIV;
          funct7 = FUNCT7_LNS_DIV;
          break;
        }

        case TOKEN_INST_32IM_LNS_SQT: {
          optype = OPTYPE_R;
          opcode = OPCODE_LNS_SQT;
          funct3 = FUNCT3_LNS_SQT;
          funct7 = FUNCT7_LNS_SQT;
          break;
        }

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

    return insts;
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

  void write(const char* filename, const RISCVEncoding& encoding) {

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
