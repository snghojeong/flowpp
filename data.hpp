#ifndef _DATA_HPP_
#define _DATA_HPP_

using data_uptr = std::unique_ptr<data>;
using format_uptr = std::unique_ptr<format>;

class data {
public:
  virtual const format_uptr format() const = 0;
  virtual void set(const data_uptr dat) = 0;
  virtual data_uptr get() const = 0;
};

#endif
