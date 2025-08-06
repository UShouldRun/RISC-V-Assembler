#ifndef __MAPPER_H__
#define __MAPPER_H__

#include <unordered_map>

#include "parser.hpp"

namespace mapper {
  uint32_t* map_inst2bin (const RISCVAST*, uint64_t&);
  uint32_t* map_data2bin (const RISCVAST*, uint64_t&);
  void      write        (const char*, const uint32_t*, const uint64_t);
}

#endif // !__MAPPER_H__
