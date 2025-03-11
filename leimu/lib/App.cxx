#include "leimu/framework.h"

#include "leimu/App.h"

static VkApplicationInfo CreateAppInfo(const std::string &name) {
  return {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = name.data(),
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = name.data(),
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_0,
  };
}

leimu::App::App(const std::string &name)
  : _name(name),
    _info(CreateAppInfo(_name)),
    _vulkan(*this) {
  if (!_glfw || !_vulkan) {
    return;
  }
}

leimu::App::~App() {
}

void leimu::App::run() {
  while (!glfwWindowShouldClose(_glfw.window())) {
    glfwPollEvents();
  }
}

const std::string &leimu::App::name() const { return _name; }
const VkApplicationInfo &leimu::App::info() const { return _info; }

bool leimu::App::operator!() const { return !_glfw || !_vulkan; }
