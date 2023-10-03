using namespace flowpp;

#ifndef _DATA_HPP_
#define _DATA_HPP_

class data <typename T> {
public:
  virtual void set(T value) = 0;
  virtual T get() const = 0;
  virtual const std::string str() const = 0;
};

#endif
