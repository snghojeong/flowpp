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
    // Notify function is now generalized for all types
    virtual void notify(DataPtr dat) override {
        std::cout << dat->get();
    }
};

// Class representing a scanner
class KeyScanner : public observable {
public:
    using DataPtr = std::unique_ptr<data<std::string>>;

    // Generates data from user input
    DataPtr generate() {
        std::string input;
        if (!(std::cin >> input)) {
            throw std::runtime_error("Input error");
        }
        return std::make_unique<data<std::string>>(input);
    }
};

// Overload the pipe operator for better readability
observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

// Overload the pipe operator for chaining observables with observers
observable& operator|(observable& lhs, ObserverPtr& rhs) {
    lhs.subscribe(rhs.get());
    return lhs;
}

int main() {
    try {
        // Instantiate engine and components using make_unique
        auto enginePtr = std::make_unique<flowpp_engine>();
        auto scannerPtr = enginePtr->instantiate<KeyScanner>();
        auto printerPtr = enginePtr->instantiate<Printer<std::string>>();
        auto counterPtr = enginePtr->instantiate<counter>();

        // Piping scanner output to printer and counter
        *scannerPtr | *printerPtr;
        *scannerPtr | *counterPtr;

        // Execute engine loop with timeout and loop count
        enginePtr->run(1000 /* timeout */, 1000 /* loop count */);

        // Output final count from counter
        std::cout << "Key count: " << counterPtr->get() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
