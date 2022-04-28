#include <iostream>

using data_uptr = std::unique_ptr<flowpp::data>;
using data_sptr = std::shared_ptr<flowpp::data>;
using fltr_uptr = std::unique_ptr<filter>;
using fltr_sptr = std::shared_ptr<filter>;
using scanner_uptr = std::unique_ptr<scanner>;
using scanner_sptr = std::shared_ptr<scanner>;

class counter : public source {
public:
  explicit counter() { _cnt = 0; }
  virtual ~counter() {}

  data_uptr generate() {
    std::cout << "_cnt: " << _cnt << std::endl;
    return std::make_unique<data>(_cnt++);
  }

  unsigned int get() {
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

  std::cout << cnter->get() << std::endl; // print 10

  return 0;
}
