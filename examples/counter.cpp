#include <iostream>

using data_uptr = std::unique_ptr<flowpp::data>;
using fltr_uptr = std::unique_ptr<filter>;

class counter : public source {
public:
  explicit counter() { _cnt = 0; }
  virtual ~counter() {}

  data_uptr generate() {
    return std::make_unique<flowpp::data>(_cnt++);
  }

  unsigned int get_cnt() {
    return _cnt;
  }

protected:
  unsigned int _cnt;
};

int main()
{
  graph_uptr cnter_graph = make_unique<graph>();;

  auto cnter = cnter_graph.get<counter>();

  cnter_graph->run(INFINITE/* timeout */, 10 /* numnber of loop */);

  std::cout << cnter->get_cnt(); // print 10

  return 0;
}
