#include "leimu/framework.h"

#include "leimu/feature/Vulkan.h"

#include "leimu/App.h"

// ReSharper disable once CppTemplateArgumentsCanBeDeduced
const std::vector<const char *> ValidationLayers = {
#if LEIMU_DEBUG
    "VK_LAYER_KHRONOS_validation"
#endif
};

#if LEIMU_DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *,
    void *);

constexpr VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo{
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                   | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = DebugCallback,
    .pUserData = nullptr
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
// ReSharper disable once CppDFAConstantFunctionResult
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *cbData,
    void *) {
  static std::map<VkDebugUtilsMessageSeverityFlagBitsEXT, std::string> lvs = {
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,        "verb"},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,           "info"},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,        "warn"},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,          "fail"},
      {VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT, "crit"},
  };

  if (severity <= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    return VK_FALSE;
  }

  std::println(
      leimu::errs(),
      "[vulkan] [validation] [{}] {}",
      lvs.at(severity),
      cbData->pMessage);

  return VK_FALSE;
}
#pragma clang diagnostic pop

// ReSharper disable once CppDeclaratorNeverUsed
// NOLINTNEXTLINE(*-reserved-identifier)
static void __vkAssert(const VkResult result, std::string op, std::string file, int line) {
  if (result != VK_SUCCESS) {
    std::println(leimu::errs(), "[vulkan] [assert] '{}' fails ({}:{})", op, file, line);
  }
}

#define vkAssert(condition) __vkAssert(condition, #condition, __FILE__, __LINE__)
#else
#define vkAssert(condition) condition
#endif

// LAYERS / EXTENSIONS

std::vector<const char *> GetInstanceExtensions() {
  u32 nExtension = 0;
  const auto vExtension = glfwGetRequiredInstanceExtensions(&nExtension);

  std::vector<const char *> extensions;
  extensions.reserve(nExtension + 2);
  extensions.assign(vExtension, vExtension + nExtension);
  extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  return extensions;
}

std::vector<const char *> GetDeviceExtensions() {
  return {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };
}

std::vector<const char *> GetLayers() {
  u32 nLayer;
  vkAssert(vkEnumerateInstanceLayerProperties(&nLayer, nullptr));

  std::vector<VkLayerProperties> layers(nLayer);
  vkAssert(vkEnumerateInstanceLayerProperties(&nLayer, layers.data()));

  for (const auto required: ValidationLayers) {
    auto found = false;
    for (const auto available: layers) {
      if (strcmp(required, available.layerName) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      std::println(leimu::errs(), "[vulkan] [layer] {} missing", required);
    }
  }

  return ValidationLayers;
}

static bool CheckDeviceExtensionSupport(const VkPhysicalDevice device) {
  u32 nExtension;
  vkAssert(vkEnumerateDeviceExtensionProperties(device, nullptr, &nExtension, nullptr));

  std::vector<VkExtensionProperties> availableExtensions(nExtension);
  vkAssert(vkEnumerateDeviceExtensionProperties(device, nullptr, &nExtension, availableExtensions.data()));

  auto extensions = GetDeviceExtensions();
  std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

  for (const auto &[name, _]: availableExtensions) {
    requiredExtensions.erase(name);
  }

  return requiredExtensions.empty();
}

// INITIALIZERS

std::optional<VkSurfaceCapabilitiesKHR> leimu::feature::GetSurfaceCapabilities(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) {

  VkSurfaceCapabilitiesKHR capabilities{};
  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.get(), surface.get(), &capabilities) != VK_SUCCESS)
    return std::nullopt;

  return capabilities;
}

std::vector<VkPresentModeKHR> leimu::feature::GetPresentModes(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept {

  u32 nPresentMode;
  vkAssert(vkGetPhysicalDeviceSurfacePresentModesKHR(device.get(), surface.get(), &nPresentMode, nullptr));

  std::vector<VkPresentModeKHR> presentModes(nPresentMode);
  if (nPresentMode != 0) {
    vkAssert(
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.get(), surface.get(), &nPresentMode, presentModes.data()));
  }

  return presentModes;
}

std::vector<VkSurfaceFormatKHR> leimu::feature::GetSurfaceFormats(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept {

  u32 nFormat;
  vkAssert(vkGetPhysicalDeviceSurfaceFormatsKHR(device.get(), surface.get(), &nFormat, nullptr));

  std::vector<VkSurfaceFormatKHR> formats(nFormat);
  if (nFormat != 0) {
    vkAssert(vkGetPhysicalDeviceSurfaceFormatsKHR(device.get(), surface.get(), &nFormat, formats.data()));
  }

  return formats;
}

VkSurfaceFormatKHR leimu::feature::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) noexcept {
  for (const auto &format: formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }

  std::println(
      errs(), "[vulkan] [surface] There is no support for SRGB B8G8R8A8 format for surface; Use surface default");
  return formats[0];
}

VkPresentModeKHR leimu::feature::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &modes,
    const bool lowEnergy) noexcept {
  // FIFO has lower energy usage than other policies
  if (lowEnergy) {
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  for (const auto &mode: modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
  }

  // Only FIFO mode is guaranteed to be available
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D leimu::feature::ChooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &cap,
    const GLFW &glfw) noexcept {
  if (cap.currentExtent.width != std::numeric_limits<u32>::max()) {
    return cap.currentExtent;
  }

  i32 w, h;
  glfwGetFramebufferSize(glfw.window(), &w, &h);
  assert(w > 0 && h > 0);

  VkExtent2D extent{
      static_cast<u32>(w),
      static_cast<u32>(h)
  };

  extent.width = std::clamp(extent.width, cap.minImageExtent.width, cap.maxImageExtent.width);
  extent.height = std::clamp(extent.height, cap.minImageExtent.height, cap.maxImageExtent.height);

  return extent;
}

int leimu::feature::RatePhysicalDeviceSuitability(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device.get(), &properties);

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(device.get(), &features);

  std::println(outs(), "[vulkan] [gpu-candidate] {}", properties.deviceName);

  auto score = 0;
  //
  // OPTIONAL
  //
  if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  // maximal size of textures (affects graphics quality)
  score += static_cast<i32>(std::min(properties.limits.maxImageDimension2D, static_cast<u32>(INT32_MAX)));

  //
  // REQUIRED
  //
  if (!features.geometryShader) {
    std::println(outs(), "[vulkan] [gpu-eliminate] '{}' has no geometry shader feature", properties.deviceName);
    score *= 0;
  }

  if (!GetQueueFamilyIndices(device, surface)) {
    std::println(
        outs(), "[vulkan] [gpu-eliminate] '{}' doesn't have required queue family", properties.deviceName);
    score *= 0;
  }

  if (!CheckDeviceExtensionSupport(device.get())) {
    std::println(
        outs(), "[vulkan] [gpu-eliminate] '{}' doesn't support required extension", properties.deviceName);
    score *= 0;
  }

  if (score > 0 &&
      (!GetSurfaceCapabilities(device, surface) ||
       GetPresentModes(device, surface).empty() ||
       GetSurfaceFormats(device, surface).empty())) {

    std::println(outs(), "[vulkan] [gpu-eliminate] There is no adequate swapchain in '{}'", properties.deviceName);
    score *= 0;
  }

  return score;
}


leimu::feature::VulkanInstance leimu::feature::CreateInstance(VkApplicationInfo info) noexcept {
  auto extensions = GetInstanceExtensions();
  for (const auto ext: extensions) {
    std::println(outs(), "[vulkan] [ext] {}", ext);
  }

  auto layers = GetLayers();
  for (const auto layer: layers) {
    std::println(outs(), "[vulkan] [layer] {}", layer);
  }

  VkInstanceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
      .pApplicationInfo = &info,
      .enabledLayerCount = static_cast<u32>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<u32>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
  };

#if LEIMU_DEBUG
  constexpr auto debugCreateInfo = DebugMessengerCreateInfo;
  createInfo.pNext = &debugCreateInfo;
#endif

  VkInstance instance;
  if (const auto result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
    std::println(errs(), "[vulkan] Couldn't create instance");
    return nullptr;
  }

  return {
      instance, [](const VkInstance self) {
        vkDestroyInstance(self, nullptr);
      }
  };
}

#if LEIMU_DEBUG
leimu::feature::VulkanDebugUtilsMessenger leimu::feature::CreateDebugUtilsMessenger(
    const VulkanInstance &instance) noexcept {
  constexpr auto info = DebugMessengerCreateInfo;

  VkDebugUtilsMessengerEXT messenger;
  if (CreateDebugUtilsMessengerEXT(instance.get(), &info, nullptr, &messenger) != VK_SUCCESS) {
    std::println(errs(), "[vulkan] Couldn't create messenger");
    return nullptr;
  }

  return {
      messenger, [=](const VkDebugUtilsMessengerEXT self) {
        DestroyDebugUtilsMessengerEXT(instance.get(), self, nullptr);
      }
  };
}
#endif

leimu::feature::VulkanSurface leimu::feature::CreateSurface(
    const VulkanInstance &instance,
    const GLFW &glfw) noexcept {
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(instance.get(), glfw.window(), nullptr, &surface) != VK_SUCCESS) {
    std::println(errs(), "[glfw] Couldn't create window surface");
    return nullptr;
  }

  return {
      surface, [=](const VkSurfaceKHR self) {
        vkDestroySurfaceKHR(instance.get(), self, nullptr);
      }
  };
}

leimu::feature::VulkanPhysicalDevice leimu::feature::GetPhysicalDevice(
    const VulkanInstance &instance,
    const VulkanSurface &surface) noexcept {
  u32 nDevice;
  vkAssert(vkEnumeratePhysicalDevices(instance.get(), &nDevice, nullptr));
  if (!nDevice) {
    std::println(errs(), "[vulkan] Couldn't find any GPU");
    return nullptr;
  }

  std::vector<VkPhysicalDevice> devices(nDevice);
  vkAssert(vkEnumeratePhysicalDevices(instance.get(), &nDevice, devices.data()));

  std::multimap<int, VkPhysicalDevice> candidates;
  for (const auto &device: devices) {
    auto score = RatePhysicalDeviceSuitability(proxy(device), surface);
    candidates.insert(std::make_pair(score, device));
  }

  if (candidates.rbegin()->first > 0) {
    const auto device = candidates.rbegin()->second;

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    std::println(outs(), "[vulkan] [gpu-selected] {}", properties.deviceName);
    return {
        device, [](const VkPhysicalDevice) {
          /* ignore */
        }
    };
  }

  std::println(errs(), "[vulkan] There is no suitable GPU");
  return nullptr;
}

leimu::feature::VulkanQueueFamilyIndices leimu::feature::GetQueueFamilyIndices(
    const VulkanPhysicalDevice &device,
    const VulkanSurface &surface) noexcept {
  u32 nFamily;
  vkGetPhysicalDeviceQueueFamilyProperties(device.get(), &nFamily, nullptr);

  std::vector<VkQueueFamilyProperties> families(nFamily);
  vkGetPhysicalDeviceQueueFamilyProperties(device.get(), &nFamily, families.data());

  u32 graphics = -1, present = -1;

  for (u32 i = 0; i < nFamily && (graphics == -1 || present == -1); i++) {
    if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics = i;
    }

    VkBool32 supportPresent = false;
    vkAssert(vkGetPhysicalDeviceSurfaceSupportKHR(device.get(), i, surface.get(), &supportPresent));
    if (supportPresent) {
      present = i;
    }
  }
  if (graphics == -1) {
    std::println(errs(), "[vulkan] Couldn't find graphics queue");
  }
  if (present == -1) {
    std::println(errs(), "[vulkan] Couldn't find present queue");
  }

  return std::make_shared<VkQueueFamilyIndices_T>(graphics, present);
}

leimu::feature::VulkanDevice leimu::feature::CreateDevice(
    const VulkanPhysicalDevice &phy,
    const VulkanSurface &surface) noexcept {
  auto indices = GetQueueFamilyIndices(phy, surface);
  assert(indices);

  const std::set families = {indices.get()->graphicsQueue, indices.get()->presentQueue};
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  queueCreateInfos.reserve(families.size());

  f32 priority = 1.0f;
  for (const u32 family: families) {
    queueCreateInfos.push_back(
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = family,
            .queueCount = 1,
            .pQueuePriorities = &priority,
        });
  }

  VkPhysicalDeviceFeatures features{};

  auto extensions = GetDeviceExtensions();

  auto layers = GetLayers();
  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),

      // For older-implementations:
      // (In up-to-date implementations, `enabledLayerCount` and `ppEnabledLayerNames` are ignored)
      .enabledLayerCount = static_cast<u32>(layers.size()),
      .ppEnabledLayerNames = layers.data(),

      .enabledExtensionCount = static_cast<u32>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),

      .pEnabledFeatures = &features,
  };

  VkDevice device;
  if (vkCreateDevice(phy.get(), &createInfo, nullptr, &device) != VK_SUCCESS) {
    std::println(errs(), "[vulkan] Couldn't create logical device");
    return nullptr;
  }

  return {
      device, [](const VkDevice self) {
        vkDestroyDevice(self, nullptr);
      }
  };
}

leimu::feature::VulkanSwapchain leimu::feature::CreateSwapchain(
    const VulkanPhysicalDevice &phy,
    const VulkanDevice &dev,
    const VulkanSurface &surface,
    const GLFW &glfw,
    const bool lowEnergy) noexcept {

  const auto capabilitiesOpt = GetSurfaceCapabilities(phy, surface);
  const auto formats = GetSurfaceFormats(phy, surface);
  const auto presents = GetPresentModes(phy, surface);

  if (!capabilitiesOpt.has_value() || formats.empty() || presents.empty()) {
    std::println(errs(), "[vulkan] Invalid swapchain detected");
    return nullptr;
  }

  const auto capabilities = capabilitiesOpt.value();

  const auto format = ChooseSwapSurfaceFormat(formats);
  const auto present = ChooseSwapPresentMode(presents, lowEnergy);
  const auto extent = ChooseSwapExtent(capabilities, glfw);

  u32 nImage = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 && nImage > capabilities.maxImageCount) {
    nImage = capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface.get(),
      .minImageCount = nImage,
      .imageFormat = format.format,
      .imageColorSpace = format.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .preTransform = capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = present,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  const auto families = GetQueueFamilyIndices(phy, surface);
  assert(families);
  const auto indices = families->indices();

  if (families->graphicsQueue != families->presentQueue) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = indices.size();
    createInfo.pQueueFamilyIndices = indices.data();
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  VkSwapchainKHR swapchain;
  if (vkCreateSwapchainKHR(dev.get(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
    std::println(errs(), "[vulkan] [surface] Failed to create swapchain");
    return nullptr;
  }

  return {
      swapchain, [=](const VkSwapchainKHR self) {
        vkDestroySwapchainKHR(dev.get(), self, nullptr);
      }
  };
}

leimu::feature::VulkanQueue leimu::feature::GetQueue(
    const VulkanDevice &device,
    const u32 index) noexcept {
  VkQueue queue;
  vkGetDeviceQueue(device.get(), index, 0, &queue);
  return {
      queue, [](const VkQueue) {
      }
  };
}

leimu::feature::Vulkan::Vulkan(const App &app) {
  VkApplicationInfo info{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .apiVersion = VK_API_VERSION_1_0,
  };
  if (!((_instance = CreateInstance(info)))) {
    std::println(errs(), "[vulkan] Failed to create instance");
    return;
  }

#if LEIMU_DEBUG
  if (!((_debugMessenger = CreateDebugUtilsMessenger(_instance)))) {
    std::println(errs(), "[vulkan] Failed to create debug messenger");
    return;
  }
#endif

  if (!((_surface = CreateSurface(_instance, app.glfw())))) {
    std::println(errs(), "[vulkan] Failed to create surface");
    return;
  }

  if (!((_physicalDevice = GetPhysicalDevice(_instance, _surface)))) {
    std::println(errs(), "[vulkan] Failed to get physical device");
    return;
  }

  if (!((_queueIndices = GetQueueFamilyIndices(_physicalDevice, _surface)))) {
    std::println(errs(), "[vulkan] Failed to get queue indices");
    return;
  }

  if (!((_device = CreateDevice(_physicalDevice, _surface)))) {
    std::println(errs(), "[vulkan] Failed to create device");
    return;
  }

  if (!((_graphicsQueue = GetQueue(_device, _queueIndices->graphicsQueue)))) {
    std::println(errs(), "[vulkan] Failed to get graphics queue");
    return;
  }

  if (!((_presentQueue = GetQueue(_device, _queueIndices->presentQueue)))) {
    std::println(errs(), "[vulkan] Failed to get present queue");
    return;
  }

  if (!((_swapchain = CreateSwapchain(_physicalDevice, _device, _surface, app.glfw(),
                                      app.config()->vulkan().latencyRelaxed)))) {
    std::println(errs(), "[vulkan] Failed to create swapchain");
    return;
  }
}

bool leimu::feature::Vulkan::operator!() const {
  return !_instance
         #if LEIMU_DEBUG
         || !_debugMessenger
         #endif
         || !_surface
         || !_physicalDevice
         || !_queueIndices
         || !_device
         || !_graphicsQueue
         || !_presentQueue
         || !_swapchain;
}
