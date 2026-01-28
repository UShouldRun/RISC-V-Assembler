#ifndef __MAPPER_H__
#define __MAPPER_H__

#include <unordered_map>

#include "parser.hpp"

namespace mapper {
  typedef struct riscv_encoding RISCVEncoding;

  uint32_t* map_inst2bin (const parser::RISCVAST*, uint32_t&, const uint32_t, uint32_t&, uint32_t&, const uint32_t);
  uint32_t* map_data2bin (const parser::RISCVAST*, uint32_t&);
  void      write        (const char*, const RISCVEncoding&);

  struct riscv_encoding {
    uint32_t
      s_insts, s_data, s_stack,
      text_addr, data_addr, stack_addr,
      *insts, *data;
  };
}

#endif // !__MAPPER_H__
