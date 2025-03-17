#pragma once

#include "leimu/framework.h"
#include "leimu/feature/Vulkan.h"

namespace leimu::render {

  using Shader = std::shared_ptr<VkShaderModule_T>;

  Shader CreateShader(
      feature::VulkanDevice device,
      size_t size,
      const void *code) noexcept;

  Shader CreateShaderFromFile(
      feature::VulkanDevice device,
      std::filesystem::path path);
}
