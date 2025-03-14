#pragma once

#include "leimu/framework.h"
#include "leimu/logging.h"

namespace leimu {
  class FeatureBase {
  public:
    explicit FeatureBase(std::string name) {
      std::println(outs(), "[init] {}", name);
    }

    virtual ~FeatureBase() = default;

    virtual bool operator!() const = 0;
    operator bool() const { return !!*this; }
  };

  template<typename TSelf>
  class Feature : public FeatureBase {
  public:
    Feature() : FeatureBase(TSelf::name()) {
    }
  };
}
