#pragma once

#include <memory>
#include <functional>
#include "observable.hpp"

template <typename T>
class observer {
public:
    // Simplifies long function signatures
    using ObservablePtr = std::shared_ptr<observable<T>>;
    using DataCallback = std::function<void(const data<T>&)>;

    virtual ~observer() = default;

    // Pure virtual interface
    virtual void subscribe(ObservablePtr subject) = 0;
    virtual void listen(DataCallback callback) = 0;
};
