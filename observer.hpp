using namespace flowpp;

#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

template <typename T>
class observer {
using data_uptr = std::unique_ptr<data<T>>;
using obsvl_uptr = std::unique_ptr<observable<T>>;
public:
  virtual void subscribe(obsvl_uptr obsvl) = 0;
  virtual void listen(std::function lambda) = 0;
  virtual data_uptr poll(uint64_t timeout = 0) = 0;
};

#endif
