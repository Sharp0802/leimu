#pragma once

#include "Context.h"

namespace leimu::context {

class GLFW final : public Context<GLFW> {
  GLFWwindow *_window;

public:
  GLFW();

  ~GLFW() override;

  [[nodiscard]] GLFWwindow *window() const { return _window; }

  bool operator!() const override;

  static std::string name() { return "GLFW"; }
};

} // namespace leimu::context
