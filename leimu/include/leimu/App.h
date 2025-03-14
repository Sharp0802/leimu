#pragma once

#include "Config.h"
#include "feature/GLFW.h"
#include "feature/Vulkan.h"

namespace leimu {
  class ContextLifetimeNote {
    std::string _finiNote;

  public:
    ContextLifetimeNote(const std::string &init, std::string fini);
    ~ContextLifetimeNote();
  };

  class App {
    ContextLifetimeNote _beginNote;

    std::string _name;
    Config _config;

    feature::GLFW _glfw;
    feature::Vulkan _vulkan;

    ContextLifetimeNote _endNote;

  public:
    App(std::string name, Config config);
    ~App();

    void run();

    [[nodiscard]] Config& config() { return _config; }
    
    [[nodiscard]] const Config& config() const { return _config; }
    [[nodiscard]] const std::string &name() const { return _name; }
    [[nodiscard]] const feature::GLFW &glfw() const { return _glfw; }
    [[nodiscard]] const feature::Vulkan &vulkan() const { return _vulkan; }

    bool operator!() const;
  };
} // namespace leimu
