#ifndef _OBSERVERABLE_HPP_
#define _OBSERVERABLE_HPP_

class observable {
  using data_uptr = std::unique_ptr<data>;
  using observer_uptr = std::unique_ptr<observer>;
  using observer_tbl_uptr = std::unique_ptr<std::map<string, observer_uptr>>;
  using data_list_uptr = std::unique_ptr<std::list<data_uptr>>;

public:
  explicit observable() { _obs_map = make_unique<std::map<string, observer_uptr>>(); _data_list = make_unique<std::list<data_uptr>>(); }
  virtual ~observable() {}

  virtual void process() {
    std::for_each(_dat_list.begin(), _dat_list.end(), [this] (data_uptr dat) {
      std::for_each(_obs_map.begin(), _obs_map.end(), [=] (std::pair<string, observer_uptr> obs) { 
        pair.second->notify(dat); 
      });
    });
  }

  void add(data_uptr dat) { _data_list.push_back(dat); }
  void add(std::string key, observer_uptr obs) { _obs_map.insert( { obs } ); }

private:
  observer_table_uptr _obs_map;
  data_list_uptr _data_list;
};

#endif
