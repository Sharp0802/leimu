#pragma once

#include "Context.h"

namespace leimu {
  class App;
}

namespace leimu::context {
  struct VkQueueFamilyIndices {
    std::optional<u32> graphics{};

    operator bool() const {
      return graphics.has_value();
    }

    bool operator!() const {
      return !this->operator bool();
    }
  };

  class Vulkan final : public Context<Vulkan> {
    VkInstance _instance;
#if LEIMU_DEBUG
    VkDebugUtilsMessengerEXT _debugMessenger;
#endif
    VkPhysicalDevice _physicalDevice;
    VkQueueFamilyIndices _queues;

  public:
    Vulkan(const App &app);

    ~Vulkan() override;

    [[nodiscard]] VkInstance instance() const { return _instance; }
    [[nodiscard]] VkPhysicalDevice physicalDevice() const { return _physicalDevice; }

    static std::string name() { return "Vulkan"; }

    bool operator!() const override;
  };
} // namespace leimu::context
