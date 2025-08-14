#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// Assume flowpp library headers are included and define
// observable, observer, data, and flowpp_engine classes.
// For this example, we'll assume they exist.
// #include "flowpp_headers.h"

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
            std::cout << "Printed: " << dat->get() << std::endl;
        }
    }
};

// Class representing a scanner
class KeyScanner : public observable {
public:
    using DataPtr = std::unique_ptr<data<std::string>>;

    DataPtr generate() {
        std::string input;
        if (std::cin >> input) {
            return std::make_unique<data<std::string>>(input);
        } else if (std::cin.eof()) {
            return nullptr; // Signal end of stream gracefully
        }
        throw std::runtime_error("Input stream error");
    }
};

// NOTE: We assume the 'counter' class can be modified to be both an 'observer'
// (to receive input) and an 'observable' (to publish its count).
// This is a common pattern in data flow libraries.

// Overloaded pipe operator for readability
observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
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
        // The scanner feeds the counter, and the counter's output feeds the printer.
        // The `counter` would need to be a component that is both an observer and an observable.
        *scanner | *counter;
        *counter | *printer;

        // Run the engine loop
        const int timeout = 1000; // in ms
        const int loop_count = 1000;
        engine->run(timeout, loop_count);

        // Output the final count
        std::cout << "\n--- Final Results ---" << std::endl;
        std::cout << "Key count: " << counter->get() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
