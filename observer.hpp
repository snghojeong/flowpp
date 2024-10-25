#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

#include <memory>
#include <functional>
#include "observable.hpp"  // Ensure this header is included for observable<T>

template <typename T>
class observer {
    using ObservablePtr = std::unique_ptr<observable<T>>;

public:
    virtual ~observer() = default;

    // Pure virtual method to subscribe to an observable
    virtual void subscribe(ObservablePtr observable) = 0;

    // Pure virtual method to listen with a callback function
    virtual void listen(const std::function<void(const std::unique_ptr<data<T>>& data)>& callback) = 0;
};

#endif // _OBSERVER_HPP_
