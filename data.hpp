#ifndef _DATA_H_
#define _DATA_H_

namespace flowpp {

class data {
public:
  explicit data() { }
  virtual ~data() { }

  virtual const format& format() = 0;
};

};

#endif
