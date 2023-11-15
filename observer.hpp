using namespace flowpp;

#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

using obsvl_uptr = std::unique_ptr<observable>;

class observer {
private:
  std::list<obsvl_uptr> obsvl_list;
public:
  virtual void subscribe(obsvl_uptr obsvl) = 0 const;
  virtual void poll() = 0;
};

#endif
