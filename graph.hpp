#ifndef _GRAPH_H_
#define _GRAPH_H_

using obsvl_uptr = std::unique_ptr<observable>;

class graph {
public:
  explicit graph() { }
  virtual ~graph() { }

  void wait(time_t timeout, unsigned long loop) {
    auto start_time = curr_time = 0;
    auto cnt = 0;
    while ((loop == 0) || ((cnt++ < loop) && (start_time + timeout > curr_time))) {
      std::for_each(_obsvl_list.begin(), _obsvl_list.end(), 
          [] (observable* obsbl) { obsbl->emit(); });
      curr_time = millis();
    }
  }

  void wait(time_t timeout) {
    wait(timeout, -1);
  }
  
  void wait(unsigned long loop) {
    wait(-1, loop);
  }

private:
  std::list<obsvl_uptr> _obsvl_list;
};

#endif
