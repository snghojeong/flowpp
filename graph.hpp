using namespace flowpp;

#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

using obsvl_uptr = std::unique_ptr<observable>;
using data_uptr<typename T> = std::unique_ptr<data<T>>;

class graph {
public:
  void run(uint64_t timeout, int loop) {
    auto start_time = 0;
    auto curr_time = 0;
    auto cnt = 0;
    while ((loop <= 0) || ((cnt++ < loop) && (start_time+timeout > curr_time))) {
      std::for_each(_obsvl_list.begin(), _obsvl_list.end(), 
          [] (obsvl_uptr obsbl) { obsbl->poll(timeout); });
      curr_time = millis();
    }
  }

  void run(uint64_t timeout) {
    run(timeout, INFINITE);
  }

  void run() {
    run(INFINITE, INFINITE);
  }

  T get<T>() = 0;

protected:
  std::list<obsvl_uptr> _obsvl_list;
};

#endif
