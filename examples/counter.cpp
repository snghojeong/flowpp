#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <concepts>

namespace flowpp {

// Interface for data packets
class data_base {
public:
    virtual ~data_base() = default;
};

template <typename T>
class data : public data_base {
public:
    explicit data(T val) : value(std::move(val)) {}
    T value; 
};

// Interface for anything that can be polled
class source {
public:
    using duration = std::chrono::milliseconds;
    using result_ptr = std::unique_ptr<data_base>;

    virtual ~source() = default;
    
    [[nodiscard]] virtual result_ptr poll(duration timeout) = 0;
};

// Concept to ensure types derived from source
template<typename T>
concept SourceType = std::derived_from<T, source>;

class graph {
public:
    // Return a weak_ptr or the shared_ptr? 
    // Returning shared_ptr is more ergonomic for the user to query state later.
    template <SourceType T, typename... Args>
    std::shared_ptr<T> add(Args&&... args) {
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        _components.push_back(comp);
        return comp;
    }

    void run(source::duration timeout, int loops) {
        if (_components.empty()) return; // Silent return is often safer than throwing here
        if (loops <= 0) throw std::invalid_argument("Loop count must be positive.");

        for (int i = 0; i < loops; ++i) {
            for (auto& c : _components) {
                // We capture the result. In a real flow, we'd pass this to a 'sink'
                auto result = c->poll(timeout);
            }
        }
    }

private:
    // Storing as shared_ptr to ensure the graph keeps components alive
    std::vector<std::shared_ptr<source>> _components;
};

// Use a specific duration type for clarity
static constexpr source::duration NO_TIMEOUT = source::duration::zero();
static constexpr source::duration WAIT_FOREVER = source::duration::max();

} // namespace flowpp

// ===== Implementation: Generic Counter =====

// Using a template for the step makes it a compile-time constant
template <std::integral T, T Step = 1>
class GenericCounter final : public flowpp::source {
public:
    explicit GenericCounter(T start = 0) : _cnt(start) {}

    // Marked 'final' to allow the compiler to potentially de-virtualize
    [[nodiscard]] result_ptr poll(duration) override {
        _cnt += Step;
        return std::make_unique<flowpp::data<T>>(_cnt);
    }

    [[nodiscard]] T get_count() const noexcept { return _cnt; }

private:
    T _cnt;
};

// ===== Main Execution =====

int main() {
    using namespace flowpp;
    
    try {
        graph pipeline;

        // Added 'final' types and template arguments for clarity
        auto c1 = pipeline.add<GenericCounter<uint32_t, 1>>(0); 
        auto c2 = pipeline.add<GenericCounter<int32_t, 2>>(100);

        pipeline.run(WAIT_FOREVER, 10);

        std::cout << "Counter 1 (Step 1): " << c1->get_count() << "\n"
                  << "Counter 2 (Step 2): " << c2->get_count() << std::endl;
                  
    } catch (const std::exception& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
