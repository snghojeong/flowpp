class counter : public source {
public:
  explicit counter() { _cnt = 0; }
  virtual ~counter() {}

  data<unsigned int> generate() {
    return data(_cnt++);
  }

  unsigned int get() {
    return _cnt;
  }

private:
  unsigned int _cnt;
};

int main()
{
  react_cpp::graph cnt_graph();

  auto cnter = cnt_graph.get<counter>();

  cnt_graph.wait(0 /* timeout */, 10 /* numnber of loop */);

  std::cout << cnter.get(); << std::endl; // print 10

  return 0;
}
