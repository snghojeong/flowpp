using namespace flowpp;

#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

using obsvl_uptr = std::unique_ptr<observable>;
using data_uptr = std::unique_ptr<data>;

class graph {
public:
  void run(time_t timeout, int loop) {
    auto start_time = 0;
    auto curr_time = 0;
    while ((loop <= 0) || ((cnt++ < loop) && (start_time > curr_time))) {
      std::for_each(_obsvl_list.begin(), _obsvl_list.end(), 
          [] (obsvl_uptr obsbl) { obsbl->emit(); });
      curr_time = millis();
    }
  }

  void run(time_t timeout) {
    run(timeout, INFINITE);
  }

  void run() {
    run(INFINITE, INFINITE);
  }

protected:
  std::list<obsvl_uptr> _obsvl_list;
};

#endif
