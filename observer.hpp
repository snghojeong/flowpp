#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

using data_uptr = std::unique_ptr<data>;

class observer {
  
public:
  virtual void notify(const data_uptr dt) = 0;
  virtual void subscribe(observable *obsvl) = 0;
};

#endif
