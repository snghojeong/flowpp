#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

#include <memory>
#include <functional>
#include "observable.hpp"  // Assuming this is the observable base class

template <typename T>
class source : public observable<T> {
    using DataPtr = std::unique_ptr<data<T>>;

public:
    // Constructor and destructor
    explicit source() = default;
    virtual ~source() = default;

    // Pure virtual function for polling data
    virtual DataPtr poll(uint64_t timeout) override = 0;

    // Pure virtual function for setting up a listener
    virtual void listen(const std::function<void(const DataPtr&)>& callback) override = 0;
};

#endif // _SOURCE_HPP_
