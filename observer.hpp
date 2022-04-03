#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

class data;
class format;
class observer {
  using data_uptr = std::unique_ptr<data>;

public:
  virtual void notify(data_uptr& data_ptr) = 0;
};

#endif
