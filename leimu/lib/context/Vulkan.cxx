#include "leimu/framework.h"

#include "leimu/context/Vulkan.h"

#include "leimu/App.h"

// ReSharper disable once CppTemplateArgumentsCanBeDeduced
const std::vector<const char*> ValidationLayers = {
#if LEIMU_DEBUG
  "VK_LAYER_KHRONOS_validation"
#endif
};

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

// ReSharper disable once CppDFAConstantFunctionResult
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
  const VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT,
  const VkDebugUtilsMessengerCallbackDataEXT *cbData,
  void *) {
  static std::map<VkDebugUtilsMessageSeverityFlagBitsEXT, std::string> lvs = {
    {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "verb"},
    {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, "info"},
    {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, "warn"},
    {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "fail"},
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

#if LEIMU_DEBUG
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

std::vector<const char*> GetInstanceExtensions() {
  u32 nExtension = 0;
  const auto vExtension = glfwGetRequiredInstanceExtensions(&nExtension);

  std::vector<const char*> extensions;
  extensions.reserve(nExtension + 2);
  extensions.assign(vExtension, vExtension + nExtension);
  extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  return extensions;
}

std::vector<const char*> GetDeviceExtensions() {
  return {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };
}

std::vector<const char*> GetLayers() {
  u32 nLayer;
  vkAssert(vkEnumerateInstanceLayerProperties(&nLayer, nullptr));

  std::vector<VkLayerProperties> layers(nLayer);
  vkAssert(vkEnumerateInstanceLayerProperties(&nLayer, layers.data()));

  for (const auto required : ValidationLayers) {
    auto found = false;
    for (const auto available : layers) {
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

// INSTANCE

static VkInstance CreateInstance(const leimu::App &app) {
  auto extensions = GetInstanceExtensions();
  for (const auto ext : extensions) {
    std::println(leimu::outs(), "[vulkan] [ext] {}", ext);
  }

  auto layers = GetLayers();
  for (const auto layer : layers) {
    std::println(leimu::outs(), "[vulkan] [layer] {}", layer);
  }

  VkInstanceCreateInfo createInfo = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
    .pApplicationInfo = &app.info(),
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
    std::println(leimu::errs(), "[vulkan] Couldn't create instance");
    return nullptr;
  }

  return instance;
}

// DEBUG MESSENGER

static VkDebugUtilsMessengerEXT CreateDebugMessenger(const leimu::context::Vulkan &vulkan) {
  constexpr auto info = DebugMessengerCreateInfo;

  VkDebugUtilsMessengerEXT messenger;
  if (CreateDebugUtilsMessengerEXT(vulkan.instance(), &info, nullptr, &messenger) != VK_SUCCESS) {
    std::println(leimu::errs(), "[vulkan] Couldn't create messenger");
    return nullptr;
  }

  return messenger;
}

// SURFACE

static VkSurfaceKHR CreateSurface(const leimu::context::GLFW &glfw, const leimu::context::Vulkan &vulkan) {
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(vulkan.instance(), glfw.window(), nullptr, &surface) != VK_SUCCESS) {
    std::println(leimu::errs(), "[glfw] Couldn't create window surface");
    return nullptr;
  }

  return surface;
}

// PHYSICAL DEVICE

static leimu::context::VkQueueFamilyIndices GetQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface) {
  leimu::context::VkQueueFamilyIndices indices;

  u32 nFamily;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &nFamily, nullptr);

  std::vector<VkQueueFamilyProperties> families(nFamily);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &nFamily, families.data());

  for (u32 i = 0; i < nFamily; i++) {
    if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = i;
    }

    VkBool32 present = false;
    vkAssert(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present));
    if (present) {
      indices.present = i;
    }

    if (indices) {
      break;
    }
  }
  if (!indices.graphics.has_value()) {
    std::println(leimu::errs(), "[vulkan] Couldn't find graphics queue family");
  }


  return indices;
}

static bool CheckDeviceExtensionSupport(const VkPhysicalDevice device) {
  u32 nExtension;
  vkAssert(vkEnumerateDeviceExtensionProperties(device, nullptr, &nExtension, nullptr));

  std::vector<VkExtensionProperties> availableExtensions(nExtension);
  vkAssert(vkEnumerateDeviceExtensionProperties(device, nullptr, &nExtension, availableExtensions.data()));

  auto extensions = GetDeviceExtensions();
  std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

  for (const auto &[name, _] : availableExtensions) {
    requiredExtensions.erase(name);
  }

  return requiredExtensions.empty();
}

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats{};
  std::vector<VkPresentModeKHR> presentModes{};

  SwapChainSupportDetails(const VkPhysicalDevice device, const VkSurfaceKHR surface) {
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities) != VK_SUCCESS) {
      return;
    }

    u32 nFormat;
    vkAssert(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &nFormat, nullptr));
    if (nFormat != 0) {
      formats.resize(nFormat);
      vkAssert(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &nFormat, formats.data()));
    }

    u32 nPresentMode;
    vkAssert(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &nPresentMode, nullptr));
    if (nPresentMode != 0) {
      presentModes.resize(nPresentMode);
      vkAssert(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &nPresentMode, presentModes.data()));
    }
  }

  operator bool() const { return !formats.empty() && !presentModes.empty(); }
  bool operator!() const { return !this->operator bool(); }
};

static int RatePhysicalDeviceSuitability(const VkPhysicalDevice device, const VkSurfaceKHR surface) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(device, &features);

  std::println(leimu::outs(), "[vulkan] [gpu-candidate] {}", properties.deviceName);

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
    std::println(leimu::outs(), "[vulkan] [gpu-eliminate] '{}' has no geometry shader feature", properties.deviceName);
    score *= 0;
  }

  if (!GetQueueFamilies(device, surface)) {
    std::println(
      leimu::outs(), "[vulkan] [gpu-eliminate] '{}' doesn't have required queue family", properties.deviceName);
    score *= 0;
  }

  if (!CheckDeviceExtensionSupport(device)) {
    std::println(
      leimu::outs(), "[vulkan] [gpu-eliminate] '{}' doesn't support required extension", properties.deviceName);
    score *= 0;
  }

  if (score > 0) {
    if (const SwapChainSupportDetails support(device, surface); !support) {
      std::println(leimu::outs(), "[vulkan] [gpu-eliminate] There is no adequate swapchain in '{}'", properties.deviceName);
      score *= 0;
    }
  }

  return score;
}

static VkPhysicalDevice GetPhysicalDevice(const leimu::context::Vulkan &vulkan) {
  u32 nDevice;
  vkAssert(vkEnumeratePhysicalDevices(vulkan.instance(), &nDevice, nullptr));
  if (!nDevice) {
    std::println(leimu::errs(), "[vulkan] Couldn't find any GPU");
    return nullptr;
  }

  std::vector<VkPhysicalDevice> devices(nDevice);
  vkAssert(vkEnumeratePhysicalDevices(vulkan.instance(), &nDevice, devices.data()));

  std::multimap<int, VkPhysicalDevice> candidates;
  for (const auto &device : devices) {
    auto score = RatePhysicalDeviceSuitability(device, vulkan.surface());
    candidates.insert(std::make_pair(score, device));
  }

  if (candidates.rbegin()->first > 0) {
    const auto device = candidates.rbegin()->second;

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    std::println(leimu::outs(), "[vulkan] [gpu-selected] {}", properties.deviceName);
    return device;
  }

  std::println(leimu::errs(), "[vulkan] There is no suitable GPU");
  return nullptr;
}

// LOGICAL DEVICE

static VkDevice CreateLogicalDevice(const leimu::context::Vulkan &vulkan) {
  auto [graphics, present] = vulkan.queueIndices();

  const std::set families = {graphics.value(), present.value()};
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  queueCreateInfos.reserve(families.size());

  f32 priority = 1.0f;
  for (const u32 family : families) {
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
  if (vkCreateDevice(vulkan.physicalDevice(), &createInfo, nullptr, &device) != VK_SUCCESS) {
    std::println(leimu::errs(), "[vulkan] Couldn't create logical device");
    return nullptr;
  }

  return device;
}

static VkQueue GetGraphicsQueue(const leimu::context::Vulkan &vulkan) {
  VkQueue queue;
  vkGetDeviceQueue(vulkan.device(), vulkan.queueIndices().graphics.value(), 0, &queue);
  return queue;
}

static VkQueue GetPresentQueue(const leimu::context::Vulkan &vulkan) {
  VkQueue queue;
  vkGetDeviceQueue(vulkan.device(), vulkan.queueIndices().present.value(), 0, &queue);
  return queue;
}

// SWAPCHAIN

static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
  for (const auto &format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }

  std::println(leimu::errs(), "[vulkan] [surface] There is no support for SRGB B8G8R8A8 format for surface; Use surface default");
  return formats[0];
}

static VkPresentModeKHR ChooseSwapPresentMode(const leimu::context::VkConfig conf, const std::vector<VkPresentModeKHR> &modes) {
  // FIFO has lower energy usage than other policies
  if (conf.powerSaving) {
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  for (const auto &mode : modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
  }

  // Only FIFO mode is guaranteed to be available
  return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &cap, const leimu::context::GLFW &glfw) {
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

static VkSwapchainKHR CreateSwapchain(const leimu::context::Vulkan &vulkan, const leimu::context::GLFW& glfw) {
  SwapChainSupportDetails support{ vulkan.physicalDevice(), vulkan.surface() };

  auto format = ChooseSwapSurfaceFormat(support.formats);
  auto present = ChooseSwapPresentMode(vulkan.config(), support.presentModes);
  auto extent = ChooseSwapExtent(support.capabilities, glfw);

  u32 nImage = support.capabilities.minImageCount + 1;
  if (support.capabilities.maxImageCount > 0 && nImage > support.capabilities.maxImageCount) {
    nImage = support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface = vulkan.surface(),
    .minImageCount = nImage,
    .imageFormat = format.format,
    .imageColorSpace = format.colorSpace,
    .imageExtent = extent,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .preTransform = support.capabilities.currentTransform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = present,
    .clipped = VK_TRUE,
    .oldSwapchain = VK_NULL_HANDLE,
  };

  const auto families = GetQueueFamilies(vulkan.physicalDevice(), vulkan.surface());
  assert(families);
  const auto indices = families.indices();

  if (families.graphics != families.present) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = indices.size();
    createInfo.pQueueFamilyIndices = indices.data();
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  VkSwapchainKHR swapchain;
  if (vkCreateSwapchainKHR(vulkan.device(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
    std::println(leimu::errs(), "[vulkan] [surface] Failed to create swapchain");
    return nullptr;
  }

  return swapchain;
}


leimu::context::Vulkan::Vulkan(const App &app)
  : _instance(CreateInstance(app)),
#if LEIMU_DEBUG
    _debugMessenger(CreateDebugMessenger(*this)),
#endif
    _surface(CreateSurface(app.glfw(), *this)),
    _physicalDevice(GetPhysicalDevice(*this)),
    _queueIndices(GetQueueFamilies(_physicalDevice, _surface)),
    _device(CreateLogicalDevice(*this)),

    _graphicsQueue(GetGraphicsQueue(*this)),
    _presentQueue(GetPresentQueue(*this)),

    _swapchain(CreateSwapchain(*this, app.glfw())){
}

leimu::context::Vulkan::~Vulkan() {
  vkDestroySwapchainKHR(_device, _swapchain, nullptr);
  vkDestroyDevice(_device, nullptr);
#if LEIMU_DEBUG
  DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
#endif
  vkDestroySurfaceKHR(_instance, _surface, nullptr);
  vkDestroyInstance(_instance, nullptr);

  std::println(outs(), "[vulkan] VkInstance destroyed");
}

bool leimu::context::Vulkan::operator!() const {
  return !_instance
#if LEIMU_DEBUG
    || !_debugMessenger
#endif
    || !_physicalDevice
    || !_surface
    || !_queueIndices
    || !_device
    || !_graphicsQueue
    || !_presentQueue
    || !_swapchain;
}
