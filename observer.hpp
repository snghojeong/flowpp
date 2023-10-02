using namespace flowpp;

#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

using obsvl_uptr = std::unique_ptr<observable>;

class observer {
public:
  virtual void subscribe(obsvl_uptr obsvl) = 0 const;
};

#endif
