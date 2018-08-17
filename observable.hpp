#ifndef _OBSERVERABLE_HPP_
#define _OBSERVERABLE_HPP_

namespace react_cpp {

class data;
class observer;

class observable {
public:
  explicit observable() {}
  virtual ~observable() {}

  virtual void emit() {
    std::for_each(_dat_list.begin(), _dat_list.end(), [] (data& dat) {
        std::for_each(_obs_list.begin(), _obs_list.end(), [] (observer& obs) { obs.notify(dat); });
        });
  }
  void add(observer& obs) { _obs_list.push_back(obs); }
  void add(data& dat) { _dat_list.push_back(dat); }

private:
  std::list<observer&> _obs_list;
  std::list<data&> _dat_list;
};

};

#endif
