#ifndef _DATA_H_
#define _DATA_H_

namespace react_cpp {

class data {
public:
  explicit data() { }
  virtual ~data() { }

  virtual std::string& type() = 0;
};

};

#endif
