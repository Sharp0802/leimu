#pragma once

#include "framework.h"

#include "Reactive.h"
#include "leimu/config/VkConfig.h"

namespace leimu {
  struct Config_T {
    Reactive<config::VkConfig> vulkan;

    Config_T(config::VkConfig vk) : vulkan(vk) {}
  };

  class Config {
    std::shared_ptr<Config_T> _config;

  public:
    template<typename... TArgs>
    Config(TArgs&&... args) : _config(std::make_shared<Config_T>(std::forward<TArgs>(args)...)) {}

    Config_T *operator->() const noexcept { return _config.operator->(); }
  };
}
