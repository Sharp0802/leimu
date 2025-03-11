#include "leimu/framework.h"

#include "leimu/logging.h"

#ifndef LOG_NO_COLOR
#define RED "\x1b[31m"
#define RST "\x1b[0m"
#else
#define RED
#define RST
#endif

std::ostream &leimu::outs() {
  std::cout << "[II] " << std::flush;

  return std::cout;
}

std::ostream &leimu::errs() {
  std::cerr << RED "[EE] " RST << std::flush;

  return std::cerr;
}
