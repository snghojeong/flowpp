#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <type_traits>

namespace flowpp {

class source;
class graph {
public:
  explicit graph() {}
  virtual ~graph() { std::for_each(_obsvl_list.begin(), _obsvl_list.end(), [] (observable* obsvl) { delete obsvl; }); }

  // get observable from graph registered the list of observable on graph.
  template <typename T>
  std::shared_ptr<T> get() {
    static_assert(std::is_base_of<observable, T>::value, "T must inherit from observable");
    auto inst = make_shared<T>();
    _obsvl_list.push_back(inst);
    return inst;
  }

  void wait(unsigned long timeout, unsigned long loop) {
    decltype(loop) cnt = 0;
    while ((loop == 0) || (cnt++ < loop))
      std::for_each(_obsvl_list.begin(), _obsvl_list.end(), 
          [] (observable* obsbl) { obsbl->emit(); });
  }

private:
  std::list<std::shared_ptr<observable>> _obsvl_list;
};

};

#endif
