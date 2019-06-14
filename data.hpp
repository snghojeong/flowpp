#ifndef _DATA_H_
#define _DATA_H_

#include <string>

namespace flowpp {

class data {
public:
  explicit data() { }
  virtual ~data() { }

  virtual const std::string& type() = 0;
};

};

#endif
