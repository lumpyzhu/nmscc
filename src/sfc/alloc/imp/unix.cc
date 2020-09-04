#if defined(__unix__) || defined(__APPLE__)

#include <stdlib.h>
#include <unistd.h>

#include "../alloc.h"

namespace sfc::alloc {

static constexpr u32 MIN_ALIGN = 8;

auto System::alloc(Layout layout) -> void* {
  if (layout.size() == 0) {
    return nullptr;
  }
  if (layout.align() <= MIN_ALIGN) {
    return ::malloc(layout.size());
  }
  return ::aligned_alloc(layout.align(), layout.size());
}

auto System::alloc_zeroed(Layout layout) -> void* {
  if (layout.size() == 0) {
    return nullptr;
  }
  if (layout.align() <= MIN_ALIGN) {
    return ::calloc(layout.nmemb(), layout.align());
  }
  const auto p = static_cast<u8*>(System::alloc(layout));
  ptr::fill(p, u8(0), layout.size());
  return p;
}

void System::dealloc(void* p, [[maybe_unused]] Layout layout) {
  if (p == nullptr) {
    return;
  }
  ::free(p);
}

auto System::realloc(void* old_ptr, Layout old_layout, usize new_size) -> void* {
  const auto new_layout = Layout::from_size_align(new_size, old_layout.align());

  if (new_size == 0) {
    System::dealloc(old_ptr, old_layout);
    return nullptr;
  }
  if (old_ptr == nullptr) {
    return System::alloc(new_layout);
  }

  if (new_layout.align() <= MIN_ALIGN) {
    return ::realloc(old_ptr, new_size);
  }

  const auto new_ptr = System::alloc(new_layout);
  ptr::copy(old_ptr % as<u8*>, new_ptr % as<u8*>, old_layout.size());
  ::free(old_ptr);
  return new_ptr;
}

}  // namespace sfc::alloc

#endif
