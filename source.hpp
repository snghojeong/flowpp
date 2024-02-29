using namespace flowpp;

#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

class data;

using data_uptr = std::unique_ptr<data>;

class source : public observable {

public:
  explicit source() {}
  virtual ~source() {}

  data_uptr poll(uint64_t timeout) = 0;
}
#endif

