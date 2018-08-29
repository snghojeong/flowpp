#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

#ifndef _OBSERVERABLE_HPP_
#include <observable.hpp>
#endif

namespace react_cpp {

class data;
class observer {
public:
  virtual void notify(data& dat) = 0;
  void subscribe(observable& obsbl) { obsbl._obs_list.push_back(this); }
};

};

#endif
