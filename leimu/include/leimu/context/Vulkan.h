#pragma once

#include "Context.h"

namespace leimu {
  class App;
}

namespace leimu::context {

  struct VkConfig {
    bool powerSaving;
  };

  struct VkQueueFamilyIndices {
    std::optional<u32> graphics{};
    std::optional<u32> present{};

    operator bool() const {
      return graphics.has_value() && present.has_value();
    }

    bool operator!() const {
      return !this->operator bool();
    }

    [[nodiscard]]
    std::array<u32, 2> indices() const {
      return {
        graphics.value(),
        present.value()
      };
    }
  };

  class Vulkan final : public Context<Vulkan> {
    VkConfig _config;
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

    VkSwapchainKHR _swapchain;

  public:
    explicit Vulkan(const App &app);

    ~Vulkan() override;

    [[nodiscard]] VkConfig config() const { return _config; }
    [[nodiscard]] VkInstance instance() const { return _instance; }
    [[nodiscard]] VkSurfaceKHR surface() const { return _surface; }
    [[nodiscard]] VkPhysicalDevice physicalDevice() const { return _physicalDevice; }
    [[nodiscard]] VkQueueFamilyIndices queueIndices() const { return _queueIndices; }
    [[nodiscard]] VkDevice device() const { return _device; }
    [[nodiscard]] VkQueue graphicsQueue() const { return _graphicsQueue; }
    [[nodiscard]] VkQueue presentQueue() const { return _presentQueue; }

    static std::string name() { return "Vulkan"; }

    bool operator!() const override;
  };
} // namespace leimu::context
