#ifndef _DATA_H_
#define _DATA_H_

namespace react_cpp {

template<typename T>
class source : public observable {
public:
  explicit source() { }
  virtual ~source() { }

  data<T> generate() = 0;
};

};

#endif
