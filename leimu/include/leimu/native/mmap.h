#pragma once

#include "leimu/framework.h"

namespace leimu::native {
  class FileMapping_T;
  using FileMapping = std::shared_ptr<FileMapping_T>;

#if __unix__
  class FileMapping_T {
    void* _p;
    size_t _size;

  public:
    FileMapping_T(void* p, size_t size) : _p(p), _size(size) {}

    [[nodiscard]] const void* ptr() const { return _p; }
    [[nodiscard]] size_t size() const { return _size; }
  };
#elif _WIN32
#error Not implemented
#else
#error Unknown operation system
#endif

  [[nodiscard]] FileMapping CreateFileMapping(std::filesystem::path path) noexcept;
}
