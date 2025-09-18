#include <iostream>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <chrono>

// ===== Minimal flowpp-like framework (for example completeness) =====
namespace flowpp {
    class data_base {
    public:
        virtual ~data_base() = default;
    };

    class source {
    public:
        using duration = std::chrono::milliseconds;
        virtual ~source() = default;
        // Return next piece of data if available (here we always produce one).
        virtual std::unique_ptr<data_base> poll(duration timeout) = 0;
    };

    template <typename T>
    class data : public data_base {
    public:
        explicit data(T val) : _value(val) {}
        [[nodiscard]] T get_value() const noexcept { return _value; }
    private:
        T _value;
    };

    class graph {
    public:
        template <typename T, typename... Args>
        std::shared_ptr<T> add(Args&&... args) {
            static_assert(std::is_base_of<source, T>::value,
                          "T must derive from flowpp::source");
            auto comp = std::make_shared<T>(std::forward<Args>(args)...);
            _components.emplace_back(comp);
            return comp;
        }

        // Run all registered components for 'loops' iterations.
        // If loops <= 0, throws to avoid accidental infinite loops in examples.
        void run(source::duration timeout, int loops) {
            if (_components.empty()) {
                throw std::runtime_error("graph::run() called with no components.");
            }
            if (loops <= 0) {
                throw std::runtime_error("graph::run() requires loops > 0 in this example.");
            }
            for (int i = 0; i < loops; ++i) {
                for (auto& c : _components) {
                    (void)c->poll(timeout); // discard in this demo
                }
            }
        }

    private:
        std::vector<std::shared_ptr<source>> _components;
    };

    // A readable "no timeout" constant using chrono.
    constexpr source::duration INFINITE = source::duration::max();
    using uint_t = unsigned int;
} // namespace flowpp

using namespace flowpp;

// ===== A generic counter source =====
template <typename T, T Step = static_cast<T>(1)>
class GenericCounter : public source {
    static_assert(std::is_integral<T>::value, "GenericCounter<T>: T must be an integral type.");
public:
    using DataPtr = std::unique_ptr<data<T>>;

    explicit GenericCounter(T start = static_cast<T>(0)) noexcept : _cnt(start) {}

    std::unique_ptr<data_base> poll(duration /*timeout*/) override {
        T current = _cnt;
        _cnt = static_cast<T>(_cnt + Step);
        return std::make_unique<data<T>>(current);
    }

    [[nodiscard]] T get_count() const noexcept { return _cnt; }

private:
    T _cnt{};
};

int main() {
    try {
        graph counter1Graph;
        graph counter2Graph;

        // Register counters
        auto counter1 = counter1Graph.add<GenericCounter<uint_t, 1>>();   // starts at 0, step 1
        auto counter2 = counter2Graph.add<GenericCounter<int, 2>>();      // starts at 0, step 2

        // Loop counts
        constexpr int counter1Loops = 10;
        constexpr int counter2Loops = 20;

        // Run
        counter1Graph.run(INFINITE, counter1Loops);
        counter2Graph.run(INFINITE, counter2Loops);

        // Print final counts (should be 10 and 40)
        std::cout << "Counter1 final count: " << counter1->get_count() << '\n';
        std::cout << "Counter2 final count: " << counter2->get_count() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
