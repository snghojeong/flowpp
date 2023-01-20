#include <iostream>

using data_uptr = std::unique_ptr<flowpp::data>;
using fltr_uptr = std::unique_ptr<flowpp::filter>;

class counter : public flowpp::source {
public:
  explicit counter() { _cnt = 0; }
  virtual ~counter() {}

  data_uptr generate() {
    return std::make_unique<flowpp::data>(_cnt++);
  }
};

class counter2 : public flowpp::source {
public:
  explicit counter2() { _cnt = 0; }
  virtual ~counter2() {}

  data_uptr generate() {
    _cnt += 2;
    return std::make_unique<flowpp::data>(_cnt);
  }
};

int main()
{
  graph_uptr cnter_graph_uptr = make_unique<graph>();

  auto cnter = cnter_graph_uptr.get<counter>();

  cnter_graph_uptr->run(INFINITE/* timeout */, 10 /* numnber of loop */);

  std::cout << cnter->get_cnt(); // print 10

  return 0;
}
