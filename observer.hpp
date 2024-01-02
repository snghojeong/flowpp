using namespace flowpp;

#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

using obsvl_uptr = std::unique_ptr<observable>;
using data_uptr = std::unique_ptr<data>;

class observer {
private:
  std::list<obsvl_uptr> obsvl_list;
public:
  virtual void subscribe(obsvl_uptr obsvl) = 0 const;
  virtual void callback(std::function func) = 0;
  virtual data_uptr poll(int64_t timeout) = 0;
};

#endif
