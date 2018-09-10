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
  friend class observer;

public:
  explicit observable() {}
  virtual ~observable() {}

  virtual void emit() {
    std::for_each(_dat_list.begin(), _dat_list.end(), [this] (std::shared_ptr<data> dat) {
        std::for_each(_obs_list.begin(), _obs_list.end(), [=] (std::shared_ptr<observer> obs) { if (this->compare(pair.first, dat->type()) obs->notify(*dat); });
        });
  }

  void add(std::shared_ptr<data> dat) { _dat_list.push_back(dat); }
  void add(std::shared_ptr<observer> obs, std::string key = "*") { _obs_map.insert( { key, obs } ); }

  virtual bool compare(const std::string& key, const std::string& dat) = 0;

private:
  std::map<std::string, std::shared_ptr<observer>> _obs_map;
  std::list<std::shared_ptr<data>> _dat_list;
};

};

#endif
