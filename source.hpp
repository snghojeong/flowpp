#pragma once

#include <memory>
#include <functional>
#include <observable.hpp>

template <typename T>
class source : public observable<T> {
    using data_uptr = std::unique_ptr<data<T>>;

public:
    explicit source() = default;
    virtual ~source() = default;

    virtual data_uptr poll(uint64_t timeout) override = 0;

    virtual void listen(const std::function<void(const data_uptr&)>& callback) override = 0;

    virtual void flush() = 0;
};
