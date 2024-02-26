using namespace flowpp;

#ifndef _DATA_HPP_
#define _DATA_HPP_

using data_uptr = std::unique_ptr<data>;

class data <typename T> {
public:
  data() { }
  virtual void set(T value) = 0;
  virtual T get() const = 0;
  virtual size_t size() const = 0;
  virtual data_uptr clone() const = 0;
  virtual void parse(std::string str) = 0;
  virtual std::string to_str() = 0;
  virtual const bytes_uptr serialize() const = 0;
  virtual const void deserialize(bytes_uptr bs) = 0;
};

#endif
