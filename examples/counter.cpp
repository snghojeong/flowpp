#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <concepts>

namespace flowpp {

class data_base {
public:
    virtual ~data_base() = default;
};

template <typename T>
class data : public data_base {
public:
    explicit data(T val) : value(val) {}
    T value; 
};

// Interface for anything that can be polled
class source {
public:
    using duration = std::chrono::milliseconds;
    virtual ~source() = default;
    virtual std::unique_ptr<data_base> poll(duration timeout) = 0;
};

// Concept to ensure types derived from source
template<typename T>
concept SourceType = std::derived_from<T, source>;

class graph {
public:
    template <SourceType T, typename... Args>
    auto add(Args&&... args) {
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        _components.push_back(comp);
        return comp;
    }

    void run(source::duration timeout, int loops) {
        if (_components.empty()) throw std::runtime_error("No components in graph.");
        if (loops <= 0)           throw std::runtime_error("Loops must be > 0.");

        while (loops--) {
            for (auto& c : _components) {
                c->poll(timeout); 
            }
        }
    }

private:
    std::vector<std::shared_ptr<source>> _components;
};

constexpr source::duration INFINITE = source::duration::max();

} // namespace flowpp

// ===== Implementation: Generic Counter =====

template <std::integral T, T Step = 1>
class GenericCounter : public flowpp::source {
public:
    explicit GenericCounter(T start = 0) : _cnt(start) {}

    std::unique_ptr<flowpp::data_base> poll(duration) override {
        return std::make_unique<flowpp::data<T>>(_cnt += Step);
    }

    [[nodiscard]] T get_count() const noexcept { return _cnt; }

private:
    T _cnt;
};

int main() {
    using namespace flowpp;
    
    try {
        graph g1, g2;

        auto c1 = g1.add<GenericCounter<unsigned int, 1>>(0); 
        auto c2 = g2.add<GenericCounter<int, 2>>(0);

        g1.run(INFINITE, 10);
        g2.run(INFINITE, 20);

        std::cout << "Counter 1: " << c1->get_count() << "\n"
                  << "Counter 2: " << c2->get_count() << std::endl;
                  
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
