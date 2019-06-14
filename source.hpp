#ifndef _SOURCE_H_
#define _SOURCE_H_

#include <observable.hpp>

namespace flowpp {

class source : public observable {
public:
  explicit source() { }
  virtual ~source() { }

  virtual data* generate() = 0;
};

};

#endif
