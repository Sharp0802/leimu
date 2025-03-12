#pragma once

#include "context/GLFW.h"
#include "context/Vulkan.h"

namespace leimu {

class ContextLifetimeNote {
  std::string _finiNote;

public:
  ContextLifetimeNote(const std::string &init, const std::string &fini);
  ~ContextLifetimeNote();
};

class App {
  ContextLifetimeNote _beginNote;

  std::string _name;
  VkApplicationInfo _info;

  context::GLFW _glfw;
  context::Vulkan _vulkan;

  ContextLifetimeNote _endNote;

public:
  App(std::string name);
  ~App();

  void run();

  [[nodiscard]] const std::string& name() const { return _name; }
  [[nodiscard]] const VkApplicationInfo& info() const { return _info; }
  [[nodiscard]] const context::GLFW& glfw() const { return _glfw; }
  [[nodiscard]] const context::Vulkan& vulkan() const { return _vulkan; }

  bool operator!() const;
};

} // namespace leimu
