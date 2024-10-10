using namespace flowpp;

using scanner_uptr = std::unique_ptr<scanner>;
using counter_uptr = std::unique_ptr<counter>;
using engine_uptr = std::unique_ptr<flowpp_engine>;
using obs_uptr = std::unique_ptr<observer>;

template<typename T>
class printer : public observer {
using data_uptr = std::unique_ptr<data<T>>;
public:
  virtual void notify(data_uptr<std::string> dat) {
    std::cout << dat->get();
  }
};

class key_scanner : public observable {
public:
  data_uptr generate() {
    std::string str;
    std::cin >> str;
    return make_unique<data<std::string>>(str); /* data is template class but compiler is able to deduce its type */
  }
};

void main() {

  auto engine_uptr = make_unique<flowpp_engine>();
  auto scanner_uptr = engine_uptr->instantiate<key_scanner>();
  auto print_uptr = engine_uptr->instantiate<txt_printer>();
  auto counter_uptr = engine_uptr->instantiate<counter>();

  scanner_uptr | print_uptr;
  scanner_uptr | counter_uptr;
  scanner_uptr | file_out_uptr;

  engine_uptr->run(1000 /* timeout */, 1000 /* number of loop */);
  
  printf("KEY count: %u", counter_uptr->get()); /* print 1000 */
  
  return;
}

