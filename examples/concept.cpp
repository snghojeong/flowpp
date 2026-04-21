#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// Assuming flowpp is a placeholder for a reactive framework
namespace fp = flowpp;

int main() try {
    // Optimization for high-throughput I/O
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Msg = fp::data<std::string>;

    struct Scanner final : fp::observable {
        std::unique_ptr<Msg> generate() {
            std::string s;
            // Improvement: Clearer logic for stream exhaustion
            if (!(std::cin >> s)) {
                if (std::cin.bad()) throw std::runtime_error("stdin I/O error");
                return nullptr; // Clean EOF
            }
            return std::make_unique<Msg>(std::move(s));
        }
    };

    struct Printer final : fp::observer {
        void notify(const Msg& d) override {
            // IMPROVEMENT: Use std::endl or explicit flush if this is the end of the pipe
            // to ensure the user sees the output in real-time.
            std::cout << d.get() << std::endl; 
        }
    };

    fp::flowpp_engine engine;

    // Instantiate components managed by the engine
    auto scanner = engine.instantiate<Scanner>();
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>();

    // IMPROVEMENT: Established a cleaner "Fluent Interface" style connection
    // if the framework supports it, otherwise keep the pointer-based subscription.
    scanner->subscribe(counter);
    counter->subscribe(printer);

    // Start processing
    engine.run();

    // Final Reporting
    std::cout << "\n--- Statistics ---\n";
    std::cout << "Processed Tokens: " << counter->get() << std::endl;

    return 0;

} catch (const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return 1;
}
