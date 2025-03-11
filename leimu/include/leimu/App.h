#pragma once

#include "context/GLFW.h"
#include "context/Vulkan.h"

namespace leimu {

class App {
  std::string _name;
  VkApplicationInfo _info;

  context::GLFW _glfw;
  context::Vulkan _vulkan;

public:
  App(const std::string &name);
  ~App();

  void run();

  const std::string& name() const;
  const VkApplicationInfo& info() const;

  bool operator!() const;
};

} // namespace leimu
