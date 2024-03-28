using namespace flowpp;

#ifndef _OBSERVERABLE_HPP_
#define _OBSERVERABLE_HPP_

class observable<typename T> {
  using data_uptr<T> = std::unique_ptr<data<T>>;
  using obs_uptr = std::unique_ptr<observer>;
  using obs_map_uptr = std::unique_ptr<std::map<string, obs_uptr>>;
  using data_list_uptr<T> = std::unique_ptr<std::list<data_uptr<T>>>;

public:
  explicit observable() { 
    _obs_map = make_unique<std::map<string, obs_uptr>>(); 
    _data_list = make_unique<std::list<data_uptr<T>>>(); 
  }

  virtual ~observable() { }

  virtual data_uptr poll(uint64_t timeout) = 0;
  virtual void listen(std::function callback) = 0;

protected:
  obs_map_uptr _obs_map;
  data_list_uptr _data_list;
};

#endif
