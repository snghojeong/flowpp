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

namespace react_cpp {

class observable {
  using string = std::string;
  using data_sptr = std::shared_ptr<data>;
  using observer_sptr = std::shared_ptr<observer>;
  friend class observer;

public:
  explicit observable() {}
  virtual ~observable() {}

  virtual void emit() {
    std::for_each(_dat_list.begin(), _dat_list.end(), [this] (data_sptr dat) {
      std::for_each(_obs_map.begin(), _obs_map.end(), [=] (std::pair<string, observer_sptr> obs) { 
        if (this->compare(pair.first, dat->type()) pair.second->notify(*dat); 
      });
    });
  }

  void add(data_sptr dat) { _dat_list.push_back(dat); }
  void add(observer_sptr obs, string key = "*") { _obs_map.insert( { key, obs } ); }

  virtual bool compare(const string& key, const string& dat) = 0;

private:
  std::map<string, observer_sptr> _obs_map;
  std::list<data_sptr> _dat_list;
};

};

#endif
