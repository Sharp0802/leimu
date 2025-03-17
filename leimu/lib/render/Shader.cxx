#include "leimu/render/Shader.h"
#include "leimu/native/mmap.h"

leimu::render::Shader leimu::render::CreateShader(
    feature::VulkanDevice device,
    size_t size,
    const void *code) noexcept {

  VkShaderModuleCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = size,
      .pCode = static_cast<const uint32_t *>(code)
  };

  VkShaderModule module;
  if (vkCreateShaderModule(device.get(), &createInfo, nullptr, &module) != VK_SUCCESS) {

#if LEIMU_DEBUG
    std::string source(static_cast<const char*>(code), size);
#endif

    std::println(
        errs(),
        "[vulkan] Failed to create shader module\n"
#if LEIMU_DEBUG
        "=== SOURCE ===\n"
        "{}\n"
        "=== END SOURCE ===",
        source
#endif
    );
    return nullptr;
  }

  return Shader{
      module, [=](const VkShaderModule self) {
        vkDestroyShaderModule(device.get(), self, nullptr);
      }
  };
}

leimu::render::Shader leimu::render::CreateShaderFromFile(
    leimu::feature::VulkanDevice device,
    std::filesystem::path path) {

  auto map = leimu::native::CreateFileMapping(path);
  if (!map) {
    std::println(errs(), "[shader] Couldn't retrieve file-mapping from '{}'", path.string());
    return nullptr;
  }

  return CreateShader(device, map->size(), map->ptr());
}
