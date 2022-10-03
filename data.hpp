#ifndef _DATA_HPP_
#define _DATA_HPP_

class data;
class format;

using data_uptr = std::unique_ptr<data>;
using fmt_uptr = std::unique_ptr<format>;

class data {
public:
  virtual const fmt_uptr format() const = 0;
  virtual void set(data_uptr dat) = 0;
  virtual std::string name() const = 0;
  virtual data_uptr clone() const = 0;
};

#endif
