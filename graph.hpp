#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <type_traits>

namespace flowpp {

class source;
class target;
class graph {
public:
  explicit graph() {}
  virtual ~graph() { }

  // get observable from graph registered the list of observable on graph.
  template <typename T>
  std::unique_ptr<T> get() {
    static_assert(std::is_base_of<observable, T>::value, "T must inherit from observable");
    auto inst = make_unique<T>();
    _obsvl_list.push_back(inst);
    return inst;
  }

  void wait(unsigned long timeout, unsigned long loop) {
    decltype(timeout) start_time = curr_time = 0;
    decltype(loop) cnt = 0;
    while ((loop == 0) || ((cnt++ < loop) && (start_time + timeout > curr_time))) {
      std::for_each(_obsvl_list.begin(), _obsvl_list.end(), 
          [] (observable* obsbl) { obsbl->emit(); });
      curr_time = millis();
    }
  }

private:
  std::list<std::unique_ptr<observable>> _obsvl_list;
};

};

#endif
