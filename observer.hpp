#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

namespace flowpp {

class data;
class observer {
  using data_sptr = std::shared_ptr<data>;

public:
  virtual void notify(data_sptr& dat) = 0;
};

};

#endif
