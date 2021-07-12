#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

namespace flowpp {

class source : public observable {
  using data_ptr = std::unique_ptr<data>;

public:
  explicit source() {}
  virtual ~source() {}

  virtual void emit() {
    this->add(this->generate());
    observable::emit();
  }

  virtual data_ptr generate() = 0; 
};

};

#endif

