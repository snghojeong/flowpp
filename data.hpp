#pragma once

#include <memory>
#include <bitset>

template <typename T>
class IData {
public:
    // Using a standard alias for the serialization format
    using BitBlock = std::bitset<8>;

    virtual ~IData() = default;

    // Core Data Access
    virtual void set(const T& value) = 0;
    virtual T get() const = 0;
    virtual size_t size() const = 0;

    // Serialization: Use values or references instead of unique_ptr 
    // for small objects like bitset<8> to avoid heap overhead.
    virtual BitBlock serialize() const = 0;
    virtual void deserialize(const BitBlock& bs) = 0;

    // Polymorphic Cloning
    virtual std::unique_ptr<IData<T>> clone() const = 0;
};
