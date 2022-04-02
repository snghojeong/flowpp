#ifndef _GRAPH_H_
#define _GRAPH_H_

class observable;
class observer;

using obsvl_uptr = std::unique_ptr<observable>;
using obsvl_sptr = std::shared_ptr<observable>;

class graph {
public:
  explicit graph() { }
  virtual ~graph() { }
  
  template <typename T>
  std::unique_ptr<T> get() {
    auto inst = make_unique<T>();
    return _obsvl_list.push_back(inst);
  }

  void wait(unsigned long timeout, unsigned long loop) {
    auto start_time = curr_time = 0;
    auto cnt = 0;
    while ((loop == 0) || ((cnt++ < loop) && (start_time + timeout > curr_time))) {
      std::for_each(_obsvl_list.begin(), _obsvl_list.end(), 
          [] (observable* obsbl) { obsbl->emit(); });
      curr_time = millis();
    }
  }

  void wait(unsigned long timeout) {
    wait(timeout, -1);
  }

private:
  std::list<obsvl_uptr> _obsvl_list;
};

#endif
