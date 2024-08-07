using namespace flowpp;

#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

class data;

using data_uptr<typename T> = std::unique_ptr<data<T>>;

class source<typename T> : public observable {

public:
  explicit source() {}
  virtual ~source() {}

  data_uptr<T> poll(uint64_t timeout) = 0;
}
#endif

