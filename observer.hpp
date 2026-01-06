#pragma once

#include <memory>
#include <functional>
#include "observable.hpp"

template <typename T>
class observer {
    using ObservablePtr = std::shared_ptr<observable<T>>;

public:
    virtual ~observer() = default;
    virtual void subscribe(ObservablePtr observable) = 0;
    virtual void listen(const std::function<void(const std::unique_ptr<data<T>>& data)>& callback) = 0;
};
