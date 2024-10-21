#ifndef _DATA_HPP_
#define _DATA_HPP_

#include <memory>
#include <string>
#include <bitset>

template <typename T>
class data {
    using StrPtr = std::unique_ptr<std::string>;
    using BsPtr = std::unique_ptr<std::bitset<8>>; // Assuming 8-bit streams for the example

public:
    data() = default;
    virtual ~data() = default;

    // Pure virtual methods for derived classes to implement
    virtual void set(const T& value) = 0;
    virtual T get() const = 0;
    virtual size_t size() const = 0;
    virtual std::unique_ptr<data<T>> clone() const = 0;
    
    // Methods for parsing and serialization
    virtual void parse(const StrPtr& str) = 0;
    virtual StrPtr to_str() const = 0;
    virtual BsPtr serialize() const = 0;
    virtual void deserialize(const BsPtr& bs) = 0;
};

#endif // _DATA_HPP_
