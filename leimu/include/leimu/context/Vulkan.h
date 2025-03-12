#pragma once

#include "Context.h"

namespace leimu {
  class App;
}

namespace leimu::context {
  struct VkQueueFamilyIndices {
    std::optional<u32> graphics{};
    std::optional<u32> present{};

    operator bool() const {
      return graphics.has_value() && present.has_value();
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
    VkSurfaceKHR _surface;
    VkPhysicalDevice _physicalDevice;
    VkQueueFamilyIndices _queueIndices;
    VkDevice _device;

    VkQueue _graphicsQueue;
    VkQueue _presentQueue;

  public:
    explicit Vulkan(const App &app);

    ~Vulkan() override;

    [[nodiscard]] VkInstance instance() const { return _instance; }
    [[nodiscard]] VkPhysicalDevice physicalDevice() const { return _physicalDevice; }
    [[nodiscard]] VkQueueFamilyIndices queueIndices() const { return _queueIndices; }
    [[nodiscard]] VkDevice device() const { return _device; }
    [[nodiscard]] VkQueue queue() const { return _graphicsQueue; }
    [[nodiscard]] VkSurfaceKHR surface() const { return _surface; }

    static std::string name() { return "Vulkan"; }

    bool operator!() const override;
  };
} // namespace leimu::context
