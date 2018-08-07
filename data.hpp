#ifndef _DATA_H_
#define _DATA_H_

template <typename T>
class data<T> {
public:
  T value() { return _val; }

private:
  T _val;
};

#endif
