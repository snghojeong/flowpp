#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

using namespace flowpp;

using ScannerPtr = std::unique_ptr<scanner>;
using CounterPtr = std::unique_ptr<counter>;
using EnginePtr = std::unique_ptr<flowpp_engine>;
using ObserverPtr = std::unique_ptr<observer>;

// Template class to print data
template<typename T>
class Printer : public observer {
    using DataPtr = std::unique_ptr<data<T>>;
public:
    void notify(DataPtr dat) override {
        if (dat) {
            std::cout << dat->get() << std::endl;
        }
    }
};

// Class representing a scanner
class KeyScanner : public observable {
public:
    using DataPtr = std::unique_ptr<data<std::string>>;

    DataPtr generate() {
        std::string input;
        if (!(std::cin >> input)) {
            throw std::runtime_error("Input error");
        }
        return std::make_unique<data<std::string>>(input);
    }
};

// Overloaded pipe operator for readability
observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

observable& operator|(observable& lhs, ObserverPtr& rhs) {
    lhs.subscribe(rhs.get());
    return lhs;
}

int main() {
    try {
        // Engine and components initialization
        auto engine = std::make_unique<flowpp_engine>();
        auto scanner = engine->instantiate<KeyScanner>();
        auto printer = engine->instantiate<Printer<std::string>>();
        auto counter = engine->instantiate<counter>();

        // Set up the data flow pipeline
        *scanner | *printer;
        *scanner | *counter;

        // Run the engine loop
        const int timeout = 1000; // in ms
        const int loop_count = 1000;
        engine->run(timeout, loop_count);

        // Output the final count
        std::cout << "Key count: " << counter->get() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
