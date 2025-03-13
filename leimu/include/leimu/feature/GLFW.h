#pragma once

#include "leimu/framework.h"
#include "Feature.h"

namespace leimu::feature {
  class GLFW final : public Feature<GLFW> {
    GLFWwindow *_window;

  public:
    GLFW();

    ~GLFW() override;

    [[nodiscard]] GLFWwindow *window() const { return _window; }

    bool operator!() const override;

    static std::string name() { return "GLFW"; }
  };
}
