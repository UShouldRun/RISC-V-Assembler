#ifndef __PARSER_PRIVATE_H__
#define __PARSER_PRIVATE_H__

#include "parser.hpp"

void _parser_parse_text (parser::RISCVAST*, uint64_t&, const lexer::RISCVToken*, const uint64_t, uint64_t&);
void _parser_parse_data (parser::RISCVAST*, uint64_t&, const lexer::RISCVToken*, const uint64_t, uint64_t&);

#endif // !__PARSER_PRIVATE_H__
