#ifndef _OBSERVABLE_HPP_
#define _OBSERVABLE_HPP_

#include <map>
#include <list>
#include <memory>
#include <string>
#include <functional>

using namespace flowpp;

template<typename T>
class observable {
    using DataPtr = std::unique_ptr<data<T>>;
    using ObserverPtr = std::unique_ptr<observer>;
    using ObserverMap = std::map<std::string, ObserverPtr>;
    using DataList = std::list<DataPtr>;

public:
    // Constructor initializes observer map and data list
    explicit observable() 
        : _obs_map(std::make_unique<ObserverMap>()), 
          _data_list(std::make_unique<DataList>()) {}

    // Virtual destructor for proper cleanup
    virtual ~observable() = default;

    // Poll method to be implemented by derived classes
    virtual DataPtr poll(uint64_t timeout) = 0;

    // Listen method to add a callback function to notify observers
    virtual void listen(const std::function<void(const DataPtr&)>& callback) = 0;

protected:
    // Protected members for the observer map and data list
    std::unique_ptr<ObserverMap> _obs_map;
    std::unique_ptr<DataList> _data_list;
};

#endif // _OBSERVABLE_HPP_
