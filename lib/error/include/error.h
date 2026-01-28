#ifndef __ERROR_H__
#define __ERROR_H__

#include <iostream>

constexpr bool DEBUG = true;
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
  } while (0);

#define log(msg, var, file, line) \
  do { \
    if (DEBUG) { \
      std::cerr << "\033[036m[INFO]\033[0m: " << msg << var \
                << " (in " << file << " at line " << line << ")\n" \
                << std::flush; \
    } \
  } while (0);

#endif // !__ERROR_H__
