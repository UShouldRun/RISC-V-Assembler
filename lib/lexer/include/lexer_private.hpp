#ifndef __LEXER_PRIVATE_H__
#define __LEXER_PRIVATE_H__

#include "lexer.hpp"

#define CHAR_QUOTE   '\"'
#define CHAR_COLON   ':'
#define CHAR_COMMA   ','
#define CHAR_PERIOD  '.'
#define CHAR_LPAREN  '('
#define CHAR_RPAREN  ')'
#define CHAR_HASH    '#'
#define CHAR_MINUS   '-'
#define CHAR_PLUS    '+'
#define CHAR_SPACE   ' '
#define CHAR_UNDER   '_'
#define CHAR_TAB     '\t'
#define CHAR_CAR_RET '\r'
#define CHAR_F_FEED  '\f'
#define CHAR_VTAB    '\v'
#define CHAR_NEWLINE '\n'
#define CHAR_END     '\0'

#define REGEX_STR_S  '\"'
#define REGEX_HEXA_S "0(x|X)"
#define REGEX_BIN_S  "0(b|B)"

#define KEYWORD_TEXT   ".text"
#define KEYWORD_DATA   ".data"
#define KEYWORD_BYTE   ".byte"
#define KEYWORD_HALF   ".half"
#define KEYWORD_WORD   ".word"
#define KEYWORD_STRING ".string"

#define KEYWORD_ZERO   "zero"
#define KEYWORD_X0     "x0"
#define KEYWORD_RA     "ra"
#define KEYWORD_X1     "x1"
#define KEYWORD_SP     "sp"
#define KEYWORD_X2     "x2"
#define KEYWORD_GP     "gp"
#define KEYWORD_X3     "x3"
#define KEYWORD_TP     "tp"
#define KEYWORD_X4     "x4"
#define KEYWORD_T0     "t0"
#define KEYWORD_X5     "x5"
#define KEYWORD_T1     "t1"
#define KEYWORD_X6     "x6"
#define KEYWORD_T2     "t2"
#define KEYWORD_X7     "x7"
#define KEYWORD_X8     "x8"
#define KEYWORD_S0     "s0"
#define KEYWORD_X9     "x9"
#define KEYWORD_S1     "s1"
#define KEYWORD_X10    "x10"
#define KEYWORD_A0     "a0"
#define KEYWORD_X11    "x11"
#define KEYWORD_A1     "a1"
#define KEYWORD_X12    "x12"
#define KEYWORD_A2     "a2"
#define KEYWORD_X13    "x13"
#define KEYWORD_A3     "a3"
#define KEYWORD_X14    "x14"
#define KEYWORD_A4     "a4"
#define KEYWORD_X15    "x15"
#define KEYWORD_A5     "a5"
#define KEYWORD_X16    "x16"
#define KEYWORD_A6     "a6"
#define KEYWORD_X17    "x17"
#define KEYWORD_A7     "a7"
#define KEYWORD_X18    "x18"
#define KEYWORD_S2     "s2"
#define KEYWORD_X19    "x19"
#define KEYWORD_S3     "s3"
#define KEYWORD_X20    "x20"
#define KEYWORD_S4     "s4"
#define KEYWORD_X21    "x21"
#define KEYWORD_S5     "s5"
#define KEYWORD_X22    "x22"
#define KEYWORD_S6     "s6"
#define KEYWORD_X23    "x23"
#define KEYWORD_S7     "s7"
#define KEYWORD_X24    "x24"
#define KEYWORD_S8     "s8"
#define KEYWORD_X25    "x25"
#define KEYWORD_S9     "s9"
#define KEYWORD_X26    "x26"
#define KEYWORD_S10    "s10"
#define KEYWORD_X27    "x27"
#define KEYWORD_S11    "s11"
#define KEYWORD_X28    "x28"
#define KEYWORD_T3     "t3"
#define KEYWORD_X29    "x29"
#define KEYWORD_T4     "t4"
#define KEYWORD_X30    "x30"
#define KEYWORD_T5     "t5"
#define KEYWORD_X31    "x31"
#define KEYWORD_T6     "t6"

#define KEYWORD_NOP    "nop"
#define KEYWORD_LI     "li"
#define KEYWORD_LA     "la"
#define KEYWORD_LUI    "lui"
#define KEYWORD_AUIPC  "auipc"
#define KEYWORD_MV     "mv"

#define KEYWORD_NEG    "neg"
#define KEYWORD_ADD    "add"
#define KEYWORD_ADDI   "addi"
#define KEYWORD_SUB    "sub"
#define KEYWORD_NOT    "not"
#define KEYWORD_AND    "and"
#define KEYWORD_ANDI   "andi"
#define KEYWORD_OR     "or"
#define KEYWORD_ORI    "ori"
#define KEYWORD_XOR    "xor"
#define KEYWORD_XORI   "xori"
#define KEYWORD_SLL    "sll"
#define KEYWORD_SLLI   "slli"
#define KEYWORD_SRL    "srl"
#define KEYWORD_SRLI   "srli"
#define KEYWORD_SRA    "sra"
#define KEYWORD_SRAI   "srai"

#define KEYWORD_MUL    "mul"
#define KEYWORD_MULH   "mulh"
#define KEYWORD_MULSU  "mulsu"
#define KEYWORD_MULU   "mulu"
#define KEYWORD_DIV    "div"
#define KEYWORD_DIVU   "divu"
#define KEYWORD_REM    "rem"
#define KEYWORD_REMU   "remu"

#define KEYWORD_L      "l"
#define KEYWORD_LB     "lb"
#define KEYWORD_LH     "lh"
#define KEYWORD_LW     "lw"
#define KEYWORD_LBU    "lbu"
#define KEYWORD_LHU    "lhu"
#define KEYWORD_S      "s"
#define KEYWORD_SB     "sb"
#define KEYWORD_SH     "sh"
#define KEYWORD_SW     "sw"

#define KEYWORD_SLT    "slt"
#define KEYWORD_SLTI   "slti"
#define KEYWORD_SLTU   "sltu"
#define KEYWORD_SLTIU  "sltiu"
#define KEYWORD_SEQZ   "seqz"
#define KEYWORD_SNEZ   "snez"
#define KEYWORD_SLTZ   "sltz"
#define KEYWORD_SGTZ   "sgtz"

#define KEYWORD_BEQ    "beq"
#define KEYWORD_BNE    "bne"
#define KEYWORD_BGT    "bgt"
#define KEYWORD_BGE    "bge"
#define KEYWORD_BLE    "ble"
#define KEYWORD_BLT    "blt"
#define KEYWORD_BGTU   "bgtu"
#define KEYWORD_BGEU   "bgeu"
#define KEYWORD_BLEU   "bleu"
#define KEYWORD_BLTU   "bltu"
#define KEYWORD_BEQZ   "beqz"
#define KEYWORD_BNEZ   "bnez"
#define KEYWORD_BLEZ   "blez"
#define KEYWORD_BGEZ   "bgez"
#define KEYWORD_BLTZ   "bltz"
#define KEYWORD_BGTZ   "bgtz"
#define KEYWORD_J      "j"
#define KEYWORD_JAL    "jal"
#define KEYWORD_JR     "jr"
#define KEYWORD_JALR   "jalr"
#define KEYWORD_CALL   "call"
#define KEYWORD_RET    "ret"

#define KEYWORD_ECALL  "ecall"
#define KEYWORD_EBREAK "ebreak"
#define KEYWORD_SRET   "sret"

#define KEYWORD_LADD   "ladd"
#define KEYWORD_LSUB   "lsub"
#define KEYWORD_LMUL   "lmul"
#define KEYWORD_LDIV   "ldiv"
#define KEYWORD_LSQT   "lsqrt"

void               _lexer_scan_line                   (lexer::RISCVToken**, uint64_t&, uint64_t&, char*, const char*, const uint32_t);
uint32_t           _lexer_scan_str                    (lexer::RISCVToken*, uint64_t&, char*, const char*, const uint32_t, const uint32_t);
uint32_t           _lexer_scan_hexa                   (lexer::RISCVToken*, uint64_t&, char*, const char*, const uint32_t, const uint32_t);
uint32_t           _lexer_scan_bin                    (lexer::RISCVToken*, uint64_t&, char*, const char*, const uint32_t, const uint32_t);
uint32_t           _lexer_scan_number                 (lexer::RISCVToken*, uint64_t&, char*, const char*, const uint32_t, const uint32_t);
uint32_t           _lexer_scan_next                   (char*, const uint32_t&, char*, const char*, const uint32_t);

uint32_t           _lexer_skip_space                  (char*);
uint32_t           _lexer_skip_comments               (char*);

lexer::RISCVToken* _lexer_tokens_realloc              (lexer::RISCVToken*, uint64_t, uint64_t&);

bool               _lexer_ch_is_regex_keyword         (const char);
bool               _lexer_ch_is_hexa                  (const char);
bool               _lexer_ch_is_digit                 (const char);
bool               _lexer_ch_is_lower_hexa            (const char);
bool               _lexer_ch_is_upper_hexa            (const char);
bool               _lexer_ch_is_bin                   (const char);
bool               _lexer_ch_is_space                 (const char);
bool               _lexer_ch_is_alpha                 (const char);

#endif // !__LEXER_PRIVATE_H__
