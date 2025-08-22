#include <iostream>
#include <memory>
#include <stdexcept>

// Assuming 'flowpp' and its components are available.
// For example purposes, we'll define minimal versions.
// In a real-world scenario, these would be provided by the flowpp library.
namespace flowpp {
    class data_base {};
    class source_base {};
    
    template <typename T>
    class data : public data_base {
    public:
        explicit data(T val) : _value(val) {}
        T get_value() const { return _value; }
    private:
        T _value;
    };
    
    class source : public source_base {
    public:
        virtual std::unique_ptr<data_base> poll(int64_t timeout) = 0;
        virtual ~source() = default;
    };
    
    class graph {
    public:
        template <typename T>
        std::shared_ptr<T> get() {
            // Placeholder for real-world graph functionality.
            // This would typically retrieve a registered component.
            // We'll just create a new one for this example.
            _component = std::make_shared<T>();
            return std::dynamic_pointer_cast<T>(_component);
        }
        
        void run(int64_t timeout, int loops) {
            if (!_component) {
                throw std::runtime_error("No component to run.");
            }
            // For this example, we'll just simulate the polling.
            for (int i = 0; i < loops; ++i) {
                _component->poll(timeout);
            }
        }
        
    private:
        std::shared_ptr<source_base> _component;
    };
    
    // Assuming this constant is part of the flowpp library.
    constexpr int64_t INFINITE = -1;
    using uint_t = unsigned int;
}

using namespace flowpp;

template <typename T, T Step = 1>
class GenericCounter : public source {
public:
    using DataPtr = std::unique_ptr<data<T>>;

    GenericCounter() : _cnt(0) {}

    DataPtr poll(int64_t /* timeout */) override {
        T current_count = _cnt;
        _cnt += Step;
        return std::make_unique<data<T>>(current_count);
    }

    T get_count() const {
        return _cnt;
    }

private:
    T _cnt;
};

int main() {
    try {
        // Create unique_ptrs for two graphs
        auto counter1Graph = std::make_unique<graph>();
        auto counter2Graph = std::make_unique<graph>();

        // Get counter instances from graphs using the generic template
        // Note: The graph's get() method would need to instantiate the template.
        auto counter1 = counter1Graph->get<GenericCounter<uint_t, 1>>();
        auto counter2 = counter2Graph->get<GenericCounter<int, 2>>();

        // Run the graphs with respective loop counts
        constexpr int counter1Loops = 10;
        constexpr int counter2Loops = 20;

        counter1Graph->run(INFINITE, counter1Loops);
        counter2Graph->run(INFINITE, counter2Loops);

        // Print the final counts
        std::cout << "Counter1 final count: " << counter1->get_count() << std::endl; // Should print 10
        std::cout << "Counter2 final count: " << counter2->get_count() << std::endl; // Should print 40
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
