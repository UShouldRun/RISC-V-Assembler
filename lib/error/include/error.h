#ifndef __ERROR_H_
#define __ERROR_H_

#include <iostream>

constexpr bool DEBUG = false;
constexpr bool FATAL = true;
constexpr bool ERROR = false;

#define error(fatal, cond, msg, var, file, line) \
  do { \
    if (cond) {\
      constexpr const char* error_type = fatal ? "FATAL" : "ERROR"; \
      std::cerr << "\033[031m[" << error_type << "]\033[0m: " << msg << var << " (in " << file << " at line " << line << ")"<< std::endl; \
      if constexpr (fatal) { \
        exit(1); \
      } \
    } \
  } while (0)

#define log(msg, var, file, line) \
  do { \
    if constexpr (DEBUG) { \
      std::cout << "\033[036m[INFO]\033[0m: " << msg << var \
                << " (in " << file << " at line " << line << ")\n"; \
    } \
  } while(0)

#endif // !__ERROR_H_
