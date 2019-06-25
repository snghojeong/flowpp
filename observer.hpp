#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

namespace flowpp {

class data;
class observer {
public:
  virtual void notify(data& dat) = 0;
};

};

#endif
