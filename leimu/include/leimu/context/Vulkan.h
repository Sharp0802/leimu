#pragma once

#include <memory>

#include "Context.h"

namespace leimu {
  class App;
}

namespace leimu::context {
  class GLFW;

  struct VkQueueFamilyIndices_T {
    u32 graphicsQueue;
    u32 presentQueue;

    VkQueueFamilyIndices_T(const u32 graphicsQueue, const u32 presentQueue)
      : graphicsQueue(graphicsQueue), presentQueue(presentQueue) {
    }

    [[nodiscard]] std::array<u32, 2> indices() const {
      return {graphicsQueue, presentQueue};
    }
  };

#define LEIMU_VK_T(t, n) using n = std::shared_ptr< t##_T >

  LEIMU_VK_T(VkInstance, VulkanInstance);
  LEIMU_VK_T(VkDebugUtilsMessengerEXT, VulkanDebugUtilsMessenger);
  LEIMU_VK_T(VkSurfaceKHR, VulkanSurface);
  LEIMU_VK_T(VkPhysicalDevice, VulkanPhysicalDevice);
  LEIMU_VK_T(VkQueueFamilyIndices, VulkanQueueFamilyIndices);
  LEIMU_VK_T(VkDevice, VulkanDevice);
  LEIMU_VK_T(VkQueue, VulkanQueue);
  LEIMU_VK_T(VkSwapchainKHR, VulkanSwapchain);

  template <typename T>
  concept VulkanProxyable =
    std::is_same_v<VkPhysicalDevice_T, T> ||
    std::is_same_v<VkQueueFamilyIndices_T, T> ||
    std::is_same_v<VkQueue_T, T>;

  template <VulkanProxyable T>
  std::shared_ptr<T> proxy(T *v) {
    return std::shared_ptr<T>(
      v, [](const T *) {
      });
  }

  [[nodiscard]] std::optional<VkSurfaceCapabilitiesKHR> GetSurfaceCapabilities(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface);
  [[nodiscard]] std::vector<VkPresentModeKHR> GetPresentModes(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept;
  [[nodiscard]] std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept;
  [[nodiscard]] VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) noexcept;
  [[nodiscard]] VkPresentModeKHR ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &modes,
    bool lowEnergy) noexcept;
  [[nodiscard]] VkExtent2D ChooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &cap,
    const GLFW &glfw) noexcept;
  [[nodiscard]] int RatePhysicalDeviceSuitability(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept;

  [[nodiscard]] VulkanInstance CreateInstance(VkApplicationInfo info) noexcept;
#if LEIMU_DEBUG
  [[nodiscard]] VulkanDebugUtilsMessenger CreateDebugUtilsMessenger(const VulkanInstance &instance) noexcept;
#endif
  [[nodiscard]] VulkanSurface CreateSurface(
    const VulkanInstance &instance,
    const GLFW &glfw) noexcept;
  [[nodiscard]] VulkanPhysicalDevice GetPhysicalDevice(
    const VulkanInstance &instance,
    const VulkanSurface &surface) noexcept;
  [[nodiscard]] VulkanQueueFamilyIndices GetQueueFamilyIndices(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept;
  [[nodiscard]] VulkanDevice CreateDevice(
    const VulkanPhysicalDevice &phy,
    const VulkanSurface &surface) noexcept;
  [[nodiscard]] VulkanSwapchain CreateSwapchain(
    const VulkanPhysicalDevice &phy,
    const VulkanDevice &dev,
    const VulkanSurface &surface,
    const GLFW &glfw,
    bool lowEnergy = false) noexcept;
  [[nodiscard]] VulkanQueue GetQueue(
    const VulkanDevice &device,
    u32 index) noexcept;

  class Vulkan final : public Context<Vulkan> {
    VulkanInstance _instance;

#if LEIMU_DEBUG
    VulkanDebugUtilsMessenger _debugMessenger;
#endif

    VulkanSurface _surface;
    VulkanPhysicalDevice _physicalDevice;
    VulkanQueueFamilyIndices _queueIndices;
    VulkanDevice _device;

    VulkanQueue _graphicsQueue;
    VulkanQueue _presentQueue;

    VulkanSwapchain _swapchain;
    std::vector<VkImage> _swapchainImages;

  public:
    explicit Vulkan(const App &app);

    ~Vulkan() override;

    static std::string name() { return "Vulkan"; }

    bool operator!() const override;
  };
} // namespace leimu::context
