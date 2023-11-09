using namespace flowpp;

#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

using data_uptr = std::unique_ptr<data>;

class source : public observable {

public:
  explicit source() {}
  virtual ~source() {}

  data_uptr pull() = 0;
  void push(data_uptr dat) = 0;
}
#endif

