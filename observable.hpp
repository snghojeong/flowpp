#ifndef _OBSERVERABLE_HPP_
#define _OBSERVERABLE_HPP_

#ifndef _OBSERVER_HPP_
#include <observer.hpp>
#endif

#include <memory>
#include <map>
#include <string>
#include <data.hpp>

class observable {
  using data_uptr = std::unique_ptr<data>;
  using data_sptr = std::shared_ptr<data>;
  using observer_uptr = std::unique_ptr<observer>;
  using observer_sptr = std::shared_ptr<observer>;
  using observer_table = std::map<string, observer_uptr>;
  friend class observer;

public:
  explicit observable() {}
  virtual ~observable() {}

  virtual void process() {
    std::for_each(_dat_list.begin(), _dat_list.end(), [this] (data_uptr dat) {
      std::for_each(_obs_map.begin(), _obs_map.end(), [=] (std::pair<string, observer_uptr> obs) { 
        pair.second->notify(dat); 
      });
    });
  }

  void add(data_uptr dat) { _dat_list.push_back(dat); }
  void add(observer_uptr obs) { _obs_map.insert( { obs } ); }

private:
  observer_table _obs_map;
  std::list<data_uptr> _dat_list;
};

#endif
