#include <iostream>

#include <observable.hpp>
#include <graph.hpp>

class counter : public flowpp::source {
public:
  explicit counter() { _cnt = 0; }
  virtual ~counter() {}

  std::unique_ptr<flowpp::data> generate() {
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
  flowpp::graph cnt_graph;

  auto cnter = cnt_graph.get<counter>();

  cnt_graph.run(INFINITE/* timeout */, 10 /* numnber of loop */);

  std::cout << cnter.get() << std::endl; // print 10

  return 0;
}
