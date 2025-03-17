#include "leimu/native/mmap.h"

#if __unix__

#include "leimu/logging.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

leimu::native::FileMapping leimu::native::CreateFileMapping(std::filesystem::path path) noexcept {
  int fd;
  if ((fd = open(path.c_str(), O_RDONLY)) == -1) {
    std::println(leimu::errs(), "[mmap.unix] Couldn't open file '{}': {}", path.string(), strerror(errno));
    return nullptr;
  }

  struct stat st;
  if (fstat(fd, &st)) {
    std::println(leimu::errs(), "[mmap.unix] Couldn't stat file '{}': {}", path.string(), strerror(errno));
    return nullptr;
  }
  if (st.st_size < 0) {
    std::println(leimu::errs(), "[mmap.unix] File size must be greater than 0: {}", path.string());
    return nullptr;
  }

  void *p;
  if (!((p = mmap(nullptr, st.st_size, PROT_READ, MAP_SHARED, fd, 0)))) {
    std::println(leimu::errs(), "[mmap.unix] Couldn't mmap file '{}': {}", path.string(), strerror(errno));
    return nullptr;
  }

  return FileMapping(
      new FileMapping_T{p, static_cast<size_t>(st.st_size)},
      [=](const FileMapping_T *self) {
        if (munmap(const_cast<void*>(self->ptr()), self->size())) {
          std::println(leimu::errs(), "[mmap.unix] Couldn't munmap file '{}': {}", path.string(), strerror(errno));
        }
      }
  );
}

#endif
