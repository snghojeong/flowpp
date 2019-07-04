#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

namespace flowpp {

class source : public observable {
  using data_sptr = std::shared_ptr<data>;

public:
  explicit source() {}
  virtual ~source() {}

  virtual void emit() {
    this->add(this->generate());
    observable::emit();
  }

  virtual data_sptr generate() = 0; 
};

};

#endif

