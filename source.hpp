#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

namespace flowpp {

class source : public observable {
  using data_uptr = std::unique_ptr<data>;

public:
  explicit source() {}
  virtual ~source() {}

  virtual void emit() {
    this->add(this->generate());
    this->emit();
  }

  virtual data_uptr generate() = 0; 
};

};

#endif

