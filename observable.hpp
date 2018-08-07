#ifndef _OBSERVERABLE_HPP_
#define _OBSERVERABLE_HPP_

class data;
class observer;

class observable {
public:
  explicit observable() {}
  virtual ~observable() {}

  virtual void emit() {
    std::transform(_dat_list.begin(), _dat_list.end(), nullptr, [] (data& dat) {
        std::transform(_obs_list.begin(), _obs_list.end(), nullptr, [] (observer& obs) { obs.notify(dat); });
        });
  }
  void add(observer& obs) { _obs_list.push_back(obs); }
  void add(data& dat) { _dat_list.push_back(dat); }

private:
  std::list<observer&> _obs_list;
  std::list<data&> _dat_list;
};

#endif
