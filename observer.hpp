using namespace flowpp;

#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

template <typename T>
class observer {
using obsvl_uptr = std::unique_ptr<observable<T>>;
public:
  virtual void subscribe(obsvl_uptr obsvl) = 0;
  virtual void listen(std::function callback) = 0;
};

#endif
