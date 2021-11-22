#ifndef _DATA_H_
#define _DATA_H_

class data {
public:
  explicit data() { }
  virtual ~data() { }

  virtual const unique_ptr<format> format() const = 0;
  virtual void set(data& dat) = 0;
};

#endif
