using namespace flowpp;

#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

using data_uptr = std::unique_ptr<data>;

class source : public observable {

public:
  explicit source() {}
  virtual ~source() {}

  data_uptr poll(int64_t timeout) = 0;
  void listen(void (*callback)) = 0;
}
#endif

