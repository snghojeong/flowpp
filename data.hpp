using namespace flowpp;

#ifndef _DATA_HPP_
#define _DATA_HPP_

template <typename T>
class data {
using std::unique_ptr<std::string> str_uptr;
using std::unique_ptr<std::bitstream> bs_uptr;
public:
  data() { }
  virtual void set(const T value) = 0;
  virtual T get() const = 0;
  virtual size_t size() const = 0;
  virtual data_uptr clone() const = 0;
  virtual void parse(const str_uptr str) = 0;
  virtual str_uptr to_str() = 0;
  virtual const bs_uptr serialize() const = 0;
  virtual const void deserialize(const bs_uptr bs) = 0;
};

#endif
