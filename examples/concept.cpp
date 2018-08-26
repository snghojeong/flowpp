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
    this->add(data_factory.get(str /* get is template method but the method is able to deduce its type */ ));
  }
};

void main() {

  react_cpp::graph key_graph();
  auto scann = key_graph.get<key_scanner>();
  auto print = key_graph.get<printer>();

  scann >> print;

  key_graph.wait(INFINITE /* timeout */, INFINITE /* number of loop */);
  
  return;
}
