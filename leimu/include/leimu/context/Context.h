#pragma once

#include "leimu/logging.h"

#include <string>

namespace leimu {

class ContextBase {
public:
  explicit ContextBase(std::string name) {
    std::println(outs(), "[init] {}", name);
  }

  virtual ~ContextBase() = default;

  virtual bool operator!() const = 0;
  operator bool() const { return !!*this; }
};

template<typename TSelf>
class Context : public ContextBase {
public:
  Context() : ContextBase(TSelf::name()) {}
};

}
