#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// Assuming flowpp is a placeholder for a reactive framework
namespace fp = flowpp;

int main() try {
    // Optimization: Fast I/O
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Msg = fp::data<std::string>;

    struct Scanner final : fp::observable {
        std::unique_ptr<Msg> generate() {
            std::string s;
            // Logic: Skip empty tokens and handle EOF/Errors cleanly
            while (std::cin >> s) {
                if (!s.empty()) {
                    return std::make_unique<Msg>(std::move(s));
                }
            }
            if (std::cin.bad()) throw std::runtime_error("stdin I/O error");
            return nullptr; 
        }
    };

    struct Printer final : fp::observer {
        // IMPROVEMENT: Added 'noexcept'. In reactive streams, the "Sink" 
        // should ideally handle its own errors or signal 'noexcept' so 
        // the engine can use optimized dispatching.
        void notify(const Msg& d) noexcept override {
            // Using \n is faster than std::endl because it avoids 
            // repetitive, expensive hardware flushes.
            std::cout << d.get() << '\n';
        }
    };

    fp::flowpp_engine engine;

    // Component instantiation managed by the engine
    auto scanner = engine.instantiate<Scanner>();
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>();

    // Pipeline setup
    scanner->subscribe(counter);
    counter->subscribe(printer);

    // Execution
    engine.run();

    // IMPROVEMENT: Explicitly flush once at the very end.
    // This ensures all buffered 'notify' data hits the terminal.
    std::cout << std::flush;

    std::cout << "--- Processed Tokens: " << counter->get() << " ---\n";
    
    return 0;

} catch (const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
    return 1;
}
