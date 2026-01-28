#ifndef __MAPPER_PRIVATE_H__
#define __MAPPER_PRIVATE_H__

#include "mapper.hpp"

typedef enum optype {
  OPTYPE_NONE,
  OPTYPE_R,
  OPTYPE_I,
  OPTYPE_S,
  OPTYPE_B,
  OPTYPE_U,
  OPTYPE_J
} OpType;

#define OPCODE_LUI     0b0110111
#define OPCODE_AUIPC   0b0010111

#define OPCODE_ADD     0b0110011
#define OPCODE_ADDI    0b0010011
#define OPCODE_SUB     0b0110011

#define OPCODE_AND     0b0110011 
#define FUNCT3_AND     0x7
#define FUNCT7_AND     0x00

#define OPCODE_ANDI    0b0010011 
#define FUNCT3_ANDI    0x7

#define OPCODE_OR      0b0110011
#define FUNCT3_OR      0x6
#define FUNCT7_OR      0x00

#define OPCODE_ORI     0b0010011 
#define FUNCT3_ORI     0x6

#define OPCODE_XOR     0b0110011
#define FUNCT3_XOR     0x4
#define FUNCT7_XOR     0x00

#define OPCODE_XORI    0b0010011 
#define FUNCT3_XORI    0x4

#define OPCODE_SLL     0b0110011
#define FUNCT3_SLL     0x1
#define FUNCT7_SLL     0x00

#define OPCODE_SLLI    0b0010011 
#define FUNCT3_SLLI    0x1

#define OPCODE_SRL     0b0110011
#define FUNCT3_SRL     0x5
#define FUNCT7_SRL     0x00

#define OPCODE_SRLI    0b0010011 
#define FUNCT3_SRLI    0x5

#define OPCODE_SRA     0b0110011
#define FUNCT3_SRA     0x5
#define FUNCT7_SRA     0x20

#define OPCODE_SRAI    0b0010011 
#define FUNCT3_SRAI    0x5

#define OPCODE_MUL     0b0110011
#define FUNCT3_MUL     0x1
#define FUNCT7_MUL     0x01

#define OPCODE_MULH    0b0110011
#define FUNCT3_MULH    0x1
#define FUNCT7_MULH    0x01

#define OPCODE_MULSU   0b0110011
#define FUNCT3_MULSU   0x2
#define FUNCT7_MULSU   0x01

#define OPCODE_MULU    0b0110011
#define FUNCT3_MULU    0x3
#define FUNCT7_MULU    0x01

#define OPCODE_DIV     0b0110011
#define FUNCT3_DIV     0x4
#define FUNCT7_DIV     0x01

#define OPCODE_DIVU    0b0110011
#define FUNCT3_DIVU    0x5
#define FUNCT7_DIVU    0x01

#define OPCODE_REM     0b0110011
#define FUNCT3_REM     0x6
#define FUNCT7_REM     0x01

#define OPCODE_REMU    0b0110011
#define FUNCT3_REMU    0x7
#define FUNCT7_REMU    0x01

#define OPCODE_LB      0b0000011 
#define FUNCT3_LB      0x0

#define OPCODE_LH      0b0000011 
#define FUNCT3_LH      0x1

#define OPCODE_LW      0b0000011 
#define FUNCT3_LW      0x2

#define OPCODE_LBU     0b0000011 
#define FUNCT3_LBU     0x2

#define OPCODE_LHU     0b0000011 
#define FUNCT3_LHU     0x2

#define OPCODE_SB      0b0100011 
#define FUNCT3_SB      0x0

#define OPCODE_SH      0b0100011 
#define FUNCT3_SH      0x1

#define OPCODE_SW      0b0100011 
#define FUNCT3_SW      0x2

#define OPCODE_SLT     0b0110011
#define FUNCT3_SLT     0x2
#define FUNCT7_SLT     0x00

#define OPCODE_SLTI    0b0010011 
#define FUNCT3_SLTI    0x2

#define OPCODE_SLTU    0b0110011
#define FUNCT3_SLTU    0x3
#define FUNCT7_SLTU    0x00

#define OPCODE_SLTIU   0b0010011 
#define FUNCT3_SLTIU   0x3

#define OPCODE_BEQ     0b1100011 
#define FUNCT3_BEQ     0x0
 
#define OPCODE_BNE     0b1100011 
#define FUNCT3_BNE     0x1

#define OPCODE_BGE     0b1100011 
#define FUNCT3_BGE     0x5

#define OPCODE_BLT     0b1100011 
#define FUNCT3_BLT     0x4

#define OPCODE_BGEU    0b1100011 
#define FUNCT3_BGEU    0x7
 
#define OPCODE_BLTU    0b1100011 
#define FUNCT3_BLTU    0x6

#define OPCODE_JAL     0b1101111

#define OPCODE_JALR    0b1100111
#define FUNCT3_JALR    0x0

#define OPCODE_OS      0b1110011
#define IMM_ECALL      0x0
#define IMM_EBREAK     0x1
#define IMM_SRET       0x102

#define OPCODE_LNS_ADD 0b0000000
#define FUNCT3_LNS_ADD 0x0
#define FUNCT7_LNS_ADD 0x00

#define OPCODE_LNS_SUB 0b0000000
#define FUNCT3_LNS_SUB 0x0
#define FUNCT7_LNS_SUB 0x00

#define OPCODE_LNS_MUL 0b0000000
#define FUNCT3_LNS_MUL 0x0
#define FUNCT7_LNS_MUL 0x00

#define OPCODE_LNS_DIV 0b0000000
#define FUNCT3_LNS_DIV 0x0
#define FUNCT7_LNS_DIV 0x00

#define OPCODE_LNS_SQT 0b0000000
#define FUNCT3_LNS_SQT 0x0
#define FUNCT7_LNS_SQT 0x00

inline uint32_t riscv_map_r_type (const uint8_t, const uint8_t, const uint8_t, const uint8_t, const uint8_t, const uint8_t);
inline uint32_t riscv_map_i_type (const uint16_t, const uint8_t, const uint8_t, const uint8_t, const uint8_t);
inline uint32_t riscv_map_s_type (const uint16_t, const uint8_t, const uint8_t, const uint8_t, const uint8_t);
inline uint32_t riscv_map_b_type (const uint16_t, const uint8_t, const uint8_t, const uint8_t, const uint8_t);
inline uint32_t riscv_map_u_type (const uint32_t, const uint8_t, const uint8_t);
inline uint32_t riscv_map_j_type (const uint32_t, const uint8_t, const uint8_t);

inline uint32_t riscv_map_relative_addr (const uint32_t, const uint32_t);

#endif // !__MAPPER_PRIVATE_H__
