#include <observable.hpp>
#include <observer.hpp>

class printer : public observer {
public:
  virtual void notify(data& dat) {
    std::cout << dat.value() << std::endl;
  }
};

class key_scanner : public observable {
public:
  void poll() {
    std::string str;
    std::cin >> str;
    this->add(data_factory.get(str));
  }
};
