#ifndef _DATA_H_
#define _DATA_H_

template <typename T>
class data<T> {
public:
  T value() { return _val; }

  data& operator = (const T& operand) {
    _var = operand;
    return *this;
  }

  data& operator = (const data<T, F>& operand) {
    _var = operand._var;
    return *this;
  }

private:
  T _val;
};

#endif
