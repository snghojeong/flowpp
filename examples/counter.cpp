#include <iostream>

#include <source.hpp>
#include <data_impl.hpp>
#include <graph.hpp>

class counter : public react_cpp::source {
public:
  explicit counter() { _cnt = 0; }
  virtual ~counter() {}

  react_cpp::data* generate() {
    std::cout << "_cnt: " << _cnt << std::endl;
    return new react_cpp::data_impl(_cnt++);
  }

  unsigned int get() {
    return _cnt;
  }

private:
  unsigned int _cnt;
};

int main()
{
  react_cpp::graph cnt_graph;

  auto& cnter = cnt_graph.get<counter>();

  cnt_graph.wait(/* 0 timeout, */ 10 /* numnber of loop */);

  std::cout << cnter.get() << std::endl; // print 10

  return 0;
}
