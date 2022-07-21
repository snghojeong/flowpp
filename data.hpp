#ifndef _DATA_HPP_
#define _DATA_HPP_

class data;

using data_uptr = std::unique_ptr<data>;
using format_uptr = std::unique_ptr<format>;

class data {
public:
  virtual const format_uptr format() const = 0;
  virtual void set(data_uptr dat) = 0;
  virtual const data_uptr get() const = 0;
  virtual std::string name() const = 0;
};

#endif
