#include <iostream>

using namespace flowpp;

class counter : public source {
using data_uptr = std::unique_ptr<data<uint_t>>;
public:
  explicit counter() { _cnt = 0; }

  data_uptr poll(int64_t timeout) {
    return std::make_unique<data_uptr>(_cnt++);
  }

private:
  uint_t _cnt;
};

class counter2 : public source {
public:
  explicit counter2() { _cnt = 0; }

  data_uptr poll(int64_t timeout) {
    _cnt += 2;
    return std::make_unique<data>(_cnt);
  }
private:
  int _cnt;
};

void main()
{
  graph_uptr cnter1_graph_uptr = make_unique<graph>();
  graph_uptr cnter2_graph_uptr = make_unique<graph>();

  auto cnter = cnter1_graph_uptr.get<counter>();
  auto cnter2 = cnter2_graph_uptr.get<counter2>();
  
  cnter1_graph_uptr->run(INFINITE/* timeout */, 10 /* numnber of loop */);
  cnter2_graph_uptr->run(INFINITE/* timeout */, 20 /* numnber of loop */);

  std::cout << cnter->get_cnt(); // print 10
  std::cout << cnter2->get_cnt(); // print 40
}
