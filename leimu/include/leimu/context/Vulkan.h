#pragma once

#include "Context.h"

namespace leimu {
  class App;
}

namespace leimu::context {

class Vulkan final : public Context<Vulkan> {
  VkInstance _instance;
#if LEIMU_DEBUG
  VkDebugUtilsMessengerEXT _debugMessenger;
#endif
  VkPhysicalDevice _physicalDevice;

public:
  Vulkan(const App &app);

  ~Vulkan() override;

  VkInstance instance() const { return _instance; }

  static std::string name() { return "Vulkan"; }

  bool operator!() const override;
};

} // namespace leimu::context
