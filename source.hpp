#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

using namespace flowpp;

using data_uptr = std::unique_ptr<data>;

class source : public observable {

public:
  explicit source() {}
  virtual ~source() {}

  virtual data_uptr generate() = 0;
  virtual data_uptr get() = 0;
}
#endif

