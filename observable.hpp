#ifndef _OBSERVERABLE_HPP_
#define _OBSERVERABLE_HPP_

#ifndef _OBSERVER_HPP_
#include <observer.hpp>
#endif

#include <memory>
#include <map>
#include <list>
#include <string>
#include <data.hpp>

namespace flowpp {

class observable {
  using string = std::string;
  using data_ptr = std::unique_ptr<data>;
  using observer_ptr = std::unique_ptr<observer>;
  friend class observer;

public:
  explicit observable() {}
  virtual ~observable() {}

  virtual void process() {
    std::for_each(_dat_list.begin(), _dat_list.end(), [this] (data_ptr dat) {
      std::for_each(_obs_map.begin(), _obs_map.end(), [=] (std::pair<filter, observer_ptr> obs) { 
        pair.second->notify(dat); 
      });
    });
  }

  void add(data_ptr dat) { _dat_list.push_back(dat); }
  void add(observer_ptr obs, filter fltr = filter()) { _obs_map.insert( { fltr, obs } ); }

private:
  std::map<string, observer_ptr> _obs_map;
  std::list<data_ptr> _dat_list;
};

};

#endif
