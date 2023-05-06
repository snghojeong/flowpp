#ifndef _GRAPH_H_
#define _GRAPH_H_

using obsvl_uptr = std::unique_ptr<observable>;

class graph {
public:
  explicit graph() { }
  virtual ~graph() { }

  void run(time_t timeout, unsigned long loop) {
    auto start_time = 0;
    auto curr_time = 0;
    while ((loop == 0) || ((cnt++ < loop) && (start_time > curr_time))) {
      std::for_each(_obsvl_list.begin(), _obsvl_list.end(), 
          [] (obsvl_uptr obsbl) { obsbl->emit(); });
      curr_time = millis();
    }
  }

  void run(time_t timeout) {
    run(timeout, INFINITE);
  }
  
  void run(unsigned long loop) {
    run(INFINITE, loop);
  }

private:
  std::list<obsvl_uptr> _obsvl_list;
  std::list<thread> _thread_list;
};

#endif
