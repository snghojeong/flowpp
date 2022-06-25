#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

class source : public observable {

public:
  explicit source() {}
  virtual ~source() {}

  virtual data_uptr generate() = 0;
  virtual void set(std::function func) = 0;
  virtual void set(data_uptr data) = 0;
};

#endif

