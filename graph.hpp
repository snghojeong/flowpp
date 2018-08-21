#ifndef _GRAPH_H_
#define _GRAPH_H_

namespace react_cpp {

class graph {
public:
template <typename T>
  T get() {
    T *inst = new T();
    obsbl_list.push_back(inst);
    return inst;
  }

};

private:
  std::list<observable*> obsbl_list;
};

#endif
