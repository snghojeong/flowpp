#ifndef _GRAPH_H_
#define _GRAPH_H_

class observable;
using obsvl_uptr = std::unique_ptr<observable>;

class graph {
public:
  explicit graph() { }
  virtual ~graph() { }

  void wait(time_t timeout, unsigned long loop) {
    auto start_time = 0;
    auto curr_time = 0;
    auto cnt = 0;
    while ((loop == 0) || ((cnt++ < loop) && (start_time > curr_time))) {
      std::for_each(_obsvl_list.begin(), _obsvl_list.end(), 
          [] (observable* obsbl) { obsbl->emit(); });
      curr_time = millis();
    }
  }

  void wait(time_t timeout) {
    wait(timeout, INFINITE);
  }
  
  void wait(unsigned long loop) {
    wait(INFINITE, loop);
  }
  
  std::unique_ptr<T> get() = 0;

private:
  std::list<obsvl_uptr> _obsvl_list;
  std::list<thread> _thread_list;
};

#endif
