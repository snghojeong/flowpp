#include <iostream>

using namespace flowpp;

using data_uptr = std::unique_ptr<data>;
using fltr_uptr = std::unique_ptr<filter>;

class counter : public source {
public:
  explicit counter() { _cnt = 0; }

  data_uptr generate() {
    return std::make_unique<data>(_cnt++);
  }

private:
  int _cnt;
};

class counter2 : public source {
public:
  explicit counter2() { _cnt = 0; }

  data_uptr generate() {
    _cnt += 2;
    return std::make_unique<data>(_cnt);
  }
};

int main()
{
  graph_uptr cnter_graph_uptr = make_unique<graph>();

  auto cnter = cnter_graph_uptr.get<counter>();
  auto cnter2 = cnter_graph_uptr.get<counter2>();

  cnter_graph_uptr->run(INFINITE/* timeout */, 10 /* numnber of loop */);

  std::cout << cnter->get_cnt(); // print 10

  return 0;
}
