using namespace flowpp;

#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

class data;

template <typename T>
class source : public observable {
using data_uptr = std::unique_ptr<data<T>>;
public:
  explicit source() {}
  virtual ~source() {}

  data_uptr poll(uint64_t timeout) = 0;
  void listen() = 0;
}
#endif

