#ifndef __MAPPER_H__
#define __MAPPER_H__

#include <unordered_map>

#include "parser.hpp"

namespace mapper {
  typedef struct riscv_encoding RISCVEncoding;

  uint32_t* map_inst2bin (const RISCVAST*, uint64_t&);
  uint32_t* map_data2bin (const RISCVAST*, uint64_t&);
  void      write        (const char*, const RISCVEncoding&);

  struct riscv_encoding {
    uint64_t s_insts, s_data;
    uint32_t *insts, *data;
  };
}

#endif // !__MAPPER_H__
