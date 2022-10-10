#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

using data_uptr = std::unique_ptr<data>;

class observer {
  
public:
  virtual void notify(const data_uptr d) = 0;
  virtual void set(const data_uptr d) = 0;
  virtual data_uptr get() = 0;
};

#endif
