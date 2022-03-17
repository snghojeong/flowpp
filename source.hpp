#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

using data_uptr = std::unique_ptr<data>;
using data_sptr = std::shared_ptr<data>;

class source : public observable {

public:
  explicit source() {}
  virtual ~source() {}

  virtual data_uptr emit() {
    return this->generate();
  }

  virtual data_uptr generate() = 0; 
  virtual data_sptr generate() = 0; 
};

#endif

