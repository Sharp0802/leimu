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

leimu::ContextLifetimeNote::ContextLifetimeNote(const std::string &init, const std::string &fini)
  : _finiNote(fini) {

  std::println(outs(), "[init] {}", init);
}

leimu::ContextLifetimeNote::~ContextLifetimeNote() {
  std::println(outs(), "[fini] {}", _finiNote);
}

leimu::App::App(std::string name)
  : _beginNote("application initializing...", "application closed"),

    _name(std::move(name)),
    _info(CreateAppInfo(_name)),
    _vulkan(*this),

    _endNote("application initialized", "application closing...") {
  if (!_glfw || !_vulkan) {
    return;
  }
}

leimu::App::~App() = default;

void leimu::App::run() {
  while (!glfwWindowShouldClose(_glfw.window())) {
    glfwPollEvents();
  }
}

bool leimu::App::operator!() const { return !_glfw || !_vulkan; }
