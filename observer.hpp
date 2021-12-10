#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

class data;
class observer {
  using data_uptr = std::unique_ptr<data>;
  using data_sptr = std::shared_ptr<data>;

public:
  virtual void notify(data_uptr& data_ptr) = 0;
  virtual void notify(data_sptr& data_ptr) = 0;
};

#endif
