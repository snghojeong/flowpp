#pragma once

#include <memory>
#include <functional>
#include <cstdint> // For uint64_t
#include "observable.hpp"

template <typename T>
class source : public observable<T> {
public:
    // Use a type alias for better readability
    using DataPtr = std::unique_ptr<data<T>>;
    
    // Better callback signature: pass by const reference to the underlying data
    // This decouples the observer from the memory management strategy.
    using Callback = std::function<void(const data<T>&)>;

    source() = default;
    virtual ~source() = default;

    // Rule of five: If you define a virtual destructor, 
    // disable or define copy/move operations.
    source(const source&) = delete;
    source& operator=(const source&) = delete;

    /**
     * @brief Blocks until data is available or timeout is reached.
     * @return DataPtr containing the result, or nullptr on timeout.
     */
    virtual DataPtr poll(uint64_t timeout_ms) override = 0;

    /**
     * @brief Registers a callback for asynchronous data arrival.
     */
    virtual void listen(const Callback& callback) override = 0;

    /**
     * @brief Forces any buffered data to be processed or cleared.
     */
    virtual void flush() = 0;
};
