using namespace flowpp;

#ifndef _OBSERVERABLE_HPP_
#define _OBSERVERABLE_HPP_

class observable {
  using data_uptr = std::unique_ptr<data>;
  using obs_uptr = std::unique_ptr<observer>;
  using obs_map_uptr = std::unique_ptr<std::map<string, observer_uptr>>;
  using data_list_uptr = std::unique_ptr<std::list<data_uptr>>;

public:
  explicit observable() { 
    _obs_map = make_unique<obs_map_uptr>(); 
    _data_list = make_unique<data_list_uptr>(); 
  }

  virtual ~observable() { }

  virtual void process() = 0;
  virtual data_uptr poll() = 0;
  virtual void listen(void (*callback)) = 0;

protected:
  obs_map_uptr _obs_map;
  data_list_uptr _data_list;
};

#endif
