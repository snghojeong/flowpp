#pragma once

#include <memory>
#include <string>
#include <bitset>

template <typename T>
class data {
    using StrPtr = std::unique_ptr<std::string>;
    using BsPtr = std::unique_ptr<std::bitset<8>>;

public:
    data() = default;
    data(const T& value);
    data(BsPtr bs);
    virtual ~data() = default;

    virtual void set(const T& value) = 0;
    virtual T get() const = 0;
    virtual size_t size() const = 0;
    virtual std::unique_ptr<data<T>> clone() const = 0;
    virtual std::unique_ptr<data<T>> move() = 0;
    virtual BsPtr serialize() const = 0;
    virtual void deserialize(const BsPtr& bs) = 0;
};
