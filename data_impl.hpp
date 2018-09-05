#ifndef _DATA_IMPL_H_
#define _DATA_IMPL_H_

#include <data.hpp>

namespace react_cpp {

template<typename T>
class data_impl : public data {
public:
  explicit data_impl(T val, const char* type) : _val(val), _type(type) { }
  virtual ~data_impl() { }

  virtual const std::string& type() {
    return _type;
  }

  data_impl& operator = (const T& operand) {
    _val = operand;
    return *this;
  }

  data_impl& operator = (const data_impl<T>& operand) {
    _val = operand._val;
    return *this;
  }

private:
  T _val;
  std::string _type;
};

};

#endif

