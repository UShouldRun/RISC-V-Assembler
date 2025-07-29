#ifndef __PARSER_PRIVATE_H__
#define __PARSER_PRIVATE_H__

#include "parser.hpp"

#define CHECK_ERROR_MSG_J \
  "parser - field type is not symbol for instruction "
#define CHECK_ERROR_MSG_JR_CALL \
  "parser - field type is not register for instruction "
#define CHECK_ERROR_MSG_JAL \
  "parser - invalid field types (should be: jal <symbol> || jal <xd>, <imm>) for instruction "
#define CHECK_ERROR_MSG_JALR \
  "parser - invalid field types (should be: jalr <xs> || jalr <xd>, <xa>, <imm>) for instruction "
#define CHECK_ERROR_MSG_1 \
  "parser - invalid field types (should be: <inst> <xd>, <imm>) for instruction "
#define CHECK_ERROR_MSG_2 \
  "parser - invalid field types (should be: <inst> <xd>, <symbol>) for instruction "
#define CHECK_ERROR_MSG_3 \
  "parser - invalid field types (should be: <inst> <xd>, <xs>) for instruction "
#define CHECK_ERROR_MSG_4 \
  "parser - invalid field types (should be: <inst> <xd>, <xa>, <symbol>) for instruction "
#define CHECK_ERROR_MSG_5 \
  "parser - invalid field types (should be: <inst> <xd>, <xa>, <imm>) for instruction "
#define CHECK_ERROR_MSG_6 \
  "parser - invalid field types (should be: <inst> <xd>, <xa>, <xb>) for instruction "
#define CHECK_ERROR_MSG_LS \
  "parser - invalid field types (should be: <inst> <xd>, <imm>(<xa>) || <l{b,h,w}> <xd>, <symbol> || <s{b,h,w}> <xd>, <symbol>, <xt>) in "

void _parser_parse_text (parser::RISCVAST**, uint64_t&, lexer::RISCVToken*, const uint64_t, uint64_t&);
void _parser_parse_data (parser::RISCVAST*, uint64_t&, lexer::RISCVToken*, const uint64_t, uint64_t&);

#endif // !__PARSER_PRIVATE_H__
