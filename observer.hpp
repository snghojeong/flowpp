#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

using data_uptr = std::unique_ptr<data>;

class observer {
  
public:
  virtual void notify(data_uptr d) = 0;
};

#endif
