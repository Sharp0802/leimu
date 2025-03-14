#include <utility>

#include "leimu/framework.h"

#include "leimu/App.h"

leimu::ContextLifetimeNote::ContextLifetimeNote(const std::string &init, std::string fini)
  : _finiNote(std::move(fini)) {

  std::println(outs(), "[init] {}", init);
}

leimu::ContextLifetimeNote::~ContextLifetimeNote() {
  std::println(outs(), "[fini] {}", _finiNote);
}

leimu::App::App(std::string name, Config config)
  : _beginNote("application initializing...", "application closed"),

    _name(std::move(name)),
    _config(std::move(config)),
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
