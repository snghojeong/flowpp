#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

class observer {
  using data_uptr = std::unique_ptr<data>;

public:
  virtual void notify(const data_uptr data_ptr) = 0;
};

#endif
