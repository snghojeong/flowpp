#include <observable.hpp>
#include <observer.hpp>

namespace flowpp {

class printer : public observer {
public:
  virtual void notify(data& dat) {
    std::cout << dat.value() << std::endl;
  }
};

class key_scanner : public observable {
public:
  data poll() {
    std::string str;
    std::cin >> str;
    return data(str); // data is template class but compiler is able to deduce its type */
  }
};

}

void main() {

  flowpp::graph key_graph();
  auto scann = key_graph.get<key_scanner>();
  auto print = key_graph.get<printer>();

  scann >> print;

  key_graph.wait(INFINITE /* timeout */, INFINITE /* number of loop */);
  
  return;
}

