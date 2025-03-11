#include "leimu/framework.h"

#include "leimu/main.h"
#include "leimu/logging.h"

static void PrintError(int error, const char* message) {
  std::println(leimu::errs(), "[glfw] {:#X}: {}", error, message);
}

bool leimu::Run() {
  glfwSetErrorCallback(PrintError);
  if (!glfwInit()) {
    return false;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow* window = glfwCreateWindow(800, 600, "vulkan", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return false;
  }

  u32 nExtension = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &nExtension, nullptr);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return true;
}
