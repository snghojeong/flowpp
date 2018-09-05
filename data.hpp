#ifndef _DATA_H_
#define _DATA_H_

#include <string>

namespace react_cpp {

class data {
public:
  explicit data() { }
  virtual ~data() { }

  virtual const std::string& type() = 0;
};

};

#endif
