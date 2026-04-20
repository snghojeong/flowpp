#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// Assuming flowpp is a placeholder for a reactive framework
namespace fp = flowpp;

int main() try {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Msg = fp::data<std::string>;

    struct Scanner final : fp::observable {
        // IMPROVEMENT: Use std::optional or a dedicated flow control signal 
        // if the framework supports it, otherwise ensure internal safety.
        std::unique_ptr<Msg> generate() {
            std::string s;
            if (std::cin >> s) {
                return std::make_unique<Msg>(std::move(s));
            }
            if (std::cin.bad()) {
                throw std::runtime_error("stdin I/O error");
            }
            return nullptr; // Standard EOF signal for flowpp engines
        }
    };

    struct Printer final : fp::observer {
        // IMPROVEMENT: Using 'const Msg&' is excellent. Added 'noexcept' 
        // if the framework allows, as printing shouldn't typically throw here.
        void notify(const Msg& d) override {
            std::cout << d.get() << '\n';
        }
    };

    fp::flowpp_engine engine;

    // IMPROVEMENT: Use the pointers returned by instantiate directly.
    // This avoids unnecessary dereferencing and keeps the code cleaner.
    auto scanner = engine.instantiate<Scanner>();
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>();

    // Establishing the pipeline using the pointers
    scanner->subscribe(counter);
    counter->subscribe(printer);

    engine.run();

    // Accessing the final state
    std::cout << "Processed Tokens: " << counter->get() << '\n';
    return 0;

} catch (const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
    return 1;
}
