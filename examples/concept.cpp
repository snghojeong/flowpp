#include <observable.hpp>
#include <observer.hpp>

using data_uptr = std::unique_ptr<data>;
using fltr_uptr = std::unique_ptr<filter>;
using scanner_uptr = std::unique_ptr<scanner>;

class printer : public observer {
public:
  virtual void notify(data_uptr d) {
    std::cout << d->val() << std::endl;
  }
};

class key_scanner : public observable {
public:
  data_uptr poll() {
    std::string str;
    std::cin >> str;
    return make_unique<data>(str); // data is template class but compiler is able to deduce its type */
  }
};

void main() {

  auto engine_uptr = make_unique<flowpp_engine>();
  auto scann_uptr = engine_uptr->instantiate<key_scanner>();
  auto print_uptr = engine_uptr->instantiate<txt_printer>();

  scann_uptr | print_uptr;
  scann_uptr | counter;

  engine_uptr->run(INFINITE /* timeout */, INFINITE /* number of loop */);
  
  return;
}

