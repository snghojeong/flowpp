#include <observable.hpp>
#include <observer.hpp>

namespace flowpp {

class printer : public observer {
public:
  virtual void notify(data& d) {
    std::cout << d.value() << std::endl;
  }
};

class key_scanner : public observable {
public:
  data poll() {
    std::string str;
    std::cin >> str;
    return data(str); // data is template class but compiler is able to deduce its type */
  }

  data interrupt() = 0;
};

}

void main() {

  auto scann = key_scanner();
  auto print = txt_printer();

  scann >> print;

  scanner.run(INFINITE /* timeout */, INFINITE /* number of loop */);
  
  return;
}

