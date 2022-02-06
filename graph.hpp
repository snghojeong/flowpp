#ifndef _GRAPH_H_
#define _GRAPH_H_

class observable;
class observer;

using obsvl_uptr = std::unique_ptr<observable>;

class graph {
public:
  explicit graph() { }
  virtual ~graph() { }
  
  template <typename T>
  std::shared_ptr<T> get() {
    static_assert(std::is_base_of<observable, T>::value, "T must inherit observable");
    auto inst = make_shared<T>();
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

private:
  std::list<obsvl_uptr> _obsvl_list;
};

#endif
