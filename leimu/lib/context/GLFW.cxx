#include "leimu/framework.h"

#include "leimu/context/GLFW.h"

static void PrintError(int error, const char *message) {
  std::println(leimu::errs(), "[glfw] {:#X}: {}", error, message);
}

leimu::context::GLFW::GLFW() : _window(nullptr) {
  glfwSetErrorCallback(PrintError);
  if (!glfwInit()) {
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  _window = glfwCreateWindow(800, 600, "vulkan", nullptr, nullptr);
  if (!_window) {
    glfwTerminate();
    return;
  }
}

leimu::context::GLFW::~GLFW() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

bool leimu::context::GLFW::operator!() const {
  return !_window;
}
