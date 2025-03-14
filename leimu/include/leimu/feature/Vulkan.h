#pragma once

#include "leimu/framework.h"

#include "Feature.h"

namespace leimu {
  class App;
}

namespace leimu::feature {
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

  struct VkSurfaceInfo_T {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR format;
    VkPresentModeKHR mode;
  };

#define LEIMU_VK_T(t, n) using n = std::shared_ptr< t##_T >

  LEIMU_VK_T(VkInstance, VulkanInstance);
  LEIMU_VK_T(VkDebugUtilsMessengerEXT, VulkanDebugUtilsMessenger);
  LEIMU_VK_T(VkSurfaceKHR, VulkanSurface);
  LEIMU_VK_T(VkSurfaceInfo, VulkanSurfaceInfo);
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
  [[nodiscard]] static VulkanSurface CreateSurface(
    const VulkanInstance &instance,
    const GLFW &glfw) noexcept;
  [[nodiscard]] static VulkanPhysicalDevice GetPhysicalDevice(
    const VulkanInstance &instance,
    const VulkanSurface &surface) noexcept;
  [[nodiscard]] static VulkanSurfaceInfo RetrieveSurfaceInfo(
      const leimu::feature::VulkanPhysicalDevice &device,
      const leimu::feature::VulkanSurface &surface,
      bool latencyRelaxed) noexcept;
  [[nodiscard]] static VulkanQueueFamilyIndices GetQueueFamilyIndices(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept;
  [[nodiscard]] static VulkanDevice CreateDevice(
    const VulkanPhysicalDevice &phy,
    const VulkanSurface &surface) noexcept;
  [[nodiscard]] static VulkanSwapchain CreateSwapchain(
    const VulkanDevice &dev,
    const VulkanSurface &surface,
    const VulkanSurfaceInfo &surfaceInfo,
    VkExtent2D extent,
    const VulkanQueueFamilyIndices& families) noexcept;
  [[nodiscard]] static VulkanQueue GetQueue(
    const VulkanDevice &device,
    u32 index) noexcept;
  [[nodiscard]] static std::vector<VkImage> CreateImageViews(VkSurfaceFormatKHR format) noexcept;

  class Vulkan final : public Feature<Vulkan> {
    VulkanInstance _instance;

#if LEIMU_DEBUG
    VulkanDebugUtilsMessenger _debugMessenger;
#endif

    VulkanSurface _surface;
    VulkanPhysicalDevice _physicalDevice;
    VulkanSurfaceInfo _surfaceInfo;

    VulkanQueueFamilyIndices _queueIndices;
    VulkanDevice _device;

    VulkanQueue _graphicsQueue;
    VulkanQueue _presentQueue;

    VulkanSwapchain _swapchain;
    std::vector<VkImage> _swapchainImages;

  public:
    explicit Vulkan(const App &app);

    static std::string name() { return "Vulkan"; }

    bool operator!() const override;
  };
} // namespace leimu::context
