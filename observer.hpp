#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

namespace flowpp {

class data;
class observer {
  using data_ptr = std::unique_ptr<data>;

public:
  virtual void notify(data_ptr& dat) = 0;
};

};

#endif
