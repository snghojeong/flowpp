#include <iostream>

#include <observable.hpp>
#include <graph.hpp>

using data_uptr = std::unique_ptr<flowpp::data>;
using data_sptr = std::shared_ptr<flowpp::data>;

class counter : public source {
public:
  explicit counter() { _cnt = 0; }
  virtual ~counter() {}

  data_uptr generate() {
    std::cout << "_cnt: " << _cnt << std::endl;
    return std::make_unique<flowpp::data_impl>(_cnt++);
  }

  unsigned int get() {
    return _cnt;
  }

protected:
  unsigned int _cnt;
};

int main()
{
  flowpp::graph cnter_graph;

  auto cnter = cnter_graph.get<counter>();

  cnter_graph.run(INFINITE/* timeout */, 10 /* numnber of loop */);

  std::cout << cnter.get() << std::endl; // print 10

  return 0;
}
