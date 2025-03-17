#pragma once

#include "leimu/framework.h"

namespace leimu::native {
  class FileMapping_T;
  using FileMapping = std::shared_ptr<FileMapping_T>;

#if __unix__
  struct FileMapping_T {
    void* p;
    size_t size;
  };
#elif _WIN32
#error Not implemented
#else
#error Unknown operation system
#endif

  [[nodiscard]] FileMapping CreateFileMapping(std::filesystem::path path) noexcept;
}
