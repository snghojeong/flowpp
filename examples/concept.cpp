#include <observable.hpp>
#include <observer.hpp>

using data_uptr = std::unique_ptr<data>;
using scanner_uptr = std::unique_ptr<scanner>;
using counter_uptr = std::unique_ptr<counter_uptr>;
using engine_uptr = std::unique_ptr<flowpp_engine>;

class printer : public observer {
public:
  virtual void notify(data_uptr dat) {
    std::cout << dat->val() << std::endl;
  }
};

class key_scanner : public observable {
public:
  data_uptr poll() {
    std::string str;
    std::cin >> str;
    return make_unique<data<std::string>>(str); // data is template class but compiler is able to deduce its type */
  }
};

void main() {

  auto engine_uptr = make_unique<flowpp_engine>();
  auto scanner_uptr = engine_uptr->instantiate<key_scanner>();
  auto print_uptr = engine_uptr->instantiate<txt_printer>();
  auto counter_uptr = engine_uptr->instantiate<counter>();

  scanner_uptr | print_uptr;
  scanner_uptr | counter_uptr;

  engine_uptr->run(10000 /* timeout */, INFINITE /* number of loop */);
  
  printf("KEY counding: %d", counter_uptr->get());
  
  return;
}

