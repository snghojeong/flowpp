#pragma once

#include <memory>
#include <functional>
#include <observable.hpp>

template <typename T>
class source : public observable<T> {
    using data_uptr = std::unique_ptr<data<T>>;

public:
    // Constructor and destructor
    explicit source() = default;
    virtual ~source() = default;

    // Pure virtual function for polling data
    virtual data_uptr poll(uint64_t timeout) override = 0;

    // Pure virtual function for setting up a listener
    virtual void listen(const std::function<void(const data_uptr&)>& callback) override = 0;
};
