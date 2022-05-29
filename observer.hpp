#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

class data;

class observer {
  using data_uptr = std::unique_ptr<data>;

public:
  virtual void notify(const data_uptr data_ptr) = 0;
  virtual data_uptr get() = 0;
  virtual auto type() = 0;
};

#endif
