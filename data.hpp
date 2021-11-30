#ifndef _DATA_H_
#define _DATA_H_

using data_uptr = std::unique_ptr<data>;
using data_sptr = std::shared_ptr<data>;
using format_uptr = std::unique_ptr<format>;

class data {
public:
  explicit data() { }
  virtual ~data() { }

  virtual const format_uptr format() const = 0;
  virtual void set(data& dat) = 0;
};

#endif
