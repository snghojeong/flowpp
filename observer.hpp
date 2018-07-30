#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

class data;

class observer {
public:
  virtual void notify(data& dt) = 0;
  void subscribe(observable& obsvl) { obsvl.add(this); };
};

#endif
