#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

namespace flowpp {

class data;
class observer {
  using data_uptr = std::unique_ptr<data>;

public:
  virtual void notify(data_uptr& dat) = 0;
};

};

#endif
