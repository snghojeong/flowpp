#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

namespace react_cpp {

class data;
class observable;

class observer {
public:
  virtual void notify(data& dat) = 0;
  void subscribe(observable& obsbl) { obsbl.add(this); }
};

};

#endif
