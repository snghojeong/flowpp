#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <type_traits>

namespace react_cpp {

class source;
class graph {
public:
  explicit graph() {}
  virtual ~graph() { std::for_each(_obsbl_list.begin(), _obsbl_list.end(), [] (observable* obsbl) { delete obsbl; }); }

  template <typename T>
  T& get() {
    static_assert(std::is_base_of<observable, T>::value, "T must inherit from observable");
    T *inst = new T();
    _obsbl_list.push_back(inst);
    if (std::is_base_of<source,T>::value) {
      _src_list.push_back(inst);
    }
    return *inst;
  }

  void wait(unsigned int loop) {
    decltype(loop) cnt = 0;
    while ((loop == 0) || (cnt++ < loop))
      std::for_each(_src_list.begin(), _src_list.end(), 
          [] (source* src) { src->add(std::shared_ptr<data>(src->generate())); });

      std::for_each(_obsbl_list.begin(), _obsbl_list.end(), 
          [] (observable* obsbl) { obsbl->emit(); });
  }

private:
  std::list<observable*> _obsbl_list;
  std::list<source*> _src_list;
};

};

#endif
