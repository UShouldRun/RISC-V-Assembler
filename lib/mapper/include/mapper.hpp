#ifndef __MAPPER_H__
#define __MAPPER_H__

namespace mapper {
  uint32_t*   map2bin (RISCVAST*, uint64_t&);
  void        write   (std::ofstream&, uint32_t*, const uint64_t);
}

#endif // !__MAPPER_H__
