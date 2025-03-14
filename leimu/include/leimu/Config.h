#pragma once

#include <memory>

namespace leimu {
  template<typename T>
  struct Options {
    static_assert(std::is_base_of_v<Options<T>, T>);
  };

  struct Config_T {

  };

  class Config {
    std::shared_ptr<Config_T> _config = std::make_shared<Config_T>();

  public:
    Config_T* operator->() const noexcept { return _config.operator->(); }
  };
}
