#ifndef _DATA_HPP_
#define _DATA_HPP_

class data;

using data_uptr = std::unique_ptr<data>;

class data {
public:
  virtual const format *format() const = 0;
  virtual void set(data_uptr dat) = 0;
  virtual T get() const = 0;
  virtual const std::string str() const = 0;
};

#endif
