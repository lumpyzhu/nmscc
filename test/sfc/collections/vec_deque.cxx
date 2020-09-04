#include "sfc/collections/vec_deque.h"

#include "sfc/log.h"
#include "sfc/test.h"

namespace sfc::collections::vec_deque {

sfc_test(push) {
  auto q = VecDeque<i32>{};
  q.push_front(12);
  q.push_front(11);
  q.push_front(10);

  q.push_back(13);
  q.push_back(14);
  q.push_back(15);

  log::info("q = {}", q);
}

}  // namespace sfc::collections::vec_deque
