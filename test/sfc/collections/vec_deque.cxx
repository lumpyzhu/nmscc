#include "sfc/collections/vec_deque.h"

#include "sfc/log.h"
#include "sfc/test.h"

namespace sfc::collections::vec_deque {

sfc_test(push) {
  auto q = VecDeque<i32>{};
  q.push_front(2);
  q.push_front(1);
  q.push_front(0);

  q.push_back(3);
  q.push_back(4);
  q.push_back(5);

  log::info("q = {}", q);
}

}  // namespace sfc::collections::vec_deque
