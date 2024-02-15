using namespace flowpp;

#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

using data_uptr = std::unique_ptr<data>;

class observer {
public:
  virtual void subscribe(obsvl_uptr obsvl) = 0;
  virtual void listen(std::function func) = 0;
  virtual data_uptr poll(uint64_t timeout) = 0;
};

#endif
