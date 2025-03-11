#include "leimu/framework.h"

#include "leimu/context/Vulkan.h"

#include "leimu/App.h"

const std::vector<const char*> RequiredLayers = {
#if LEIMU_DEBUG
  "VK_LAYER_KHRONOS_validation"
#endif
};

std::vector<const char*> GetExtensions() {
  u32 nExtension = 0;
  const auto vExtension = glfwGetRequiredInstanceExtensions(&nExtension);

  std::vector<const char*> extensions;
  extensions.reserve(nExtension + 2);
  extensions.assign(vExtension, vExtension + nExtension);
  extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  return extensions;
}

std::vector<const char*> GetLayers() {
  u32 nLayer;
  vkEnumerateInstanceLayerProperties(&nLayer, nullptr);

  std::vector<VkLayerProperties> layers(nLayer);
  vkEnumerateInstanceLayerProperties(&nLayer, layers.data());

  for (const auto required : RequiredLayers) {
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

  return RequiredLayers;
}

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

static VkInstance CreateInstance(const leimu::App &app) {
  auto extensions = GetExtensions();
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
  auto debugCreateInfo = DebugMessengerCreateInfo;
  createInfo.pNext = &debugCreateInfo;
#endif

  VkInstance instance;
  if (const auto result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
    std::println(leimu::errs(), "[vulkan] Couldn't create instance");
    return nullptr;
  }

  return instance;
}

static VkDebugUtilsMessengerEXT CreateDebugMessenger(const leimu::context::Vulkan &vulkan) {
  auto info = DebugMessengerCreateInfo;

  VkDebugUtilsMessengerEXT messenger;
  if (CreateDebugUtilsMessengerEXT(vulkan.instance(), &info, nullptr, &messenger) != VK_SUCCESS) {
    std::println(leimu::errs(), "[vulkan] Couldn't create messenger");
    return nullptr;
  }

  return messenger;
}

static leimu::context::VkQueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device) {
  leimu::context::VkQueueFamilyIndices indices;

  u32 nFamily;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &nFamily, nullptr);

  std::vector<VkQueueFamilyProperties> families(nFamily);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &nFamily, families.data());

  for (u32 i = 0; i < nFamily; i++) {
    if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = i;
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

int RatePhysicalDeviceSuitability(VkPhysicalDevice device) {
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

  if (!GetQueueFamilies(device)) {
    std::println(leimu::outs(), "[vulkan] [gpu-eliminate] '{}' doesn't have required queue family", properties.deviceName);
    score *= 0;
  }

  return score;
}

static VkPhysicalDevice GetPhysicalDevice(const leimu::context::Vulkan &vulkan) {
  u32 nDevice;
  vkEnumeratePhysicalDevices(vulkan.instance(), &nDevice, nullptr);
  if (!nDevice) {
    std::println(leimu::errs(), "[vulkan] Couldn't find any GPU");
    return nullptr;
  }

  std::vector<VkPhysicalDevice> devices(nDevice);
  vkEnumeratePhysicalDevices(vulkan.instance(), &nDevice, devices.data());

  std::multimap<int, VkPhysicalDevice> candidates;
  for (const auto &device : devices) {
    auto score = RatePhysicalDeviceSuitability(device);
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

leimu::context::Vulkan::Vulkan(const App &app)
  : _instance(CreateInstance(app)),
#if LEIMU_DEBUG
    _debugMessenger(CreateDebugMessenger(*this)),
#endif
    _physicalDevice(GetPhysicalDevice(*this)),
    _queues(GetQueueFamilies(_physicalDevice)) {
}

leimu::context::Vulkan::~Vulkan() {
#if LEIMU_DEBUG
  DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
#endif
  vkDestroyInstance(_instance, nullptr);
}

bool leimu::context::Vulkan::operator!() const {
  return !_instance
#if LEIMU_DEBUG
    || !_debugMessenger
#endif
    || !_physicalDevice;
}
