#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

using data_uptr = std::unique_ptr<data>;

class source : public observable {

public:
  explicit source() {}
  virtual ~source() {}

  virtual data_uptr generate() = 0;
  virtual void set(data_uptr d) = 0;
  virtual fmt_uptr get_format() = 0;
}

#endif

