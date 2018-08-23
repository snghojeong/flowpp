#ifndef _GRAPH_H_
#define _GRAPH_H_

namespace react_cpp {

class graph {
public:
  explicit graph() {}
  virtual ~graph() { std::for_each(_obsbl_list.begin(), _obsbl_list.end(), [] (observable* obsbl) { delete obsbl; }); }

  template <typename T>
  T get() {
    T *inst = new T();
    _obsbl_list.push_back(inst);
    return inst;
  }

  void wait() {
    while (1)
      std::for_each(_obsbl_list.begin(), _obsbl_list.end(), 
          [] (observable* obsbl) { obsbl->emit(); });
  }

};

private:
  std::list<observable*> _obsbl_list;
};

#endif
