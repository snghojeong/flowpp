#ifndef _OBSERVERABLE_HPP_
#define _OBSERVERABLE_HPP_

class observable {
public:
  explicit observable() {}
  virtual ~observable() {}

  virtual void emit() = 0;
};

#endif
