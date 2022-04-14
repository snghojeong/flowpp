#ifndef _DATA_H_
#define _DATA_H_

using data_uptr = std::unique_ptr<data>;
using data_sptr = std::shared_ptr<data>;
using format_uptr = std::unique_ptr<format>;
using format_sptr = std::shared_ptr<format>;

class data {
public:
  virtual const format_uptr format() const = 0;
  virtual void set(const data_uptr dat) = 0;
  virtual void set(const data_sptr dat) = 0;
  virtual data_uptr get() const = 0;
};

#endif
