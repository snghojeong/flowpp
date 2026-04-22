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
        std::unique_ptr<Msg> generate() {
            std::string s;
            // IMPROVEMENT: Added validation to skip empty tokens or whitespace.
            // This ensures the pipeline only processes meaningful data.
            while (std::cin >> s) {
                if (!s.empty()) { 
                    return std::make_unique<Msg>(std::move(s));
                }
            }

            if (std::cin.bad()) {
                throw std::runtime_error("stdin I/O error");
            }
            return nullptr; // EOF
        }
    };

    struct Printer final : fp::observer {
        void notify(const Msg& d) override {
            // Using '\n' with an explicit flush is often faster than std::endl
            // while still ensuring the user sees the output.
            std::cout << d.get() << "\n" << std::flush;
        }
    };

    fp::flowpp_engine engine;

    // Component instantiation
    auto scanner = engine.instantiate<Scanner>();
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>();

    // Pipeline setup
    scanner->subscribe(counter);
    counter->subscribe(printer);

    engine.run();

    // Final result output
    std::cout << "--- Processed Tokens: " << counter->get() << " ---\n";
    
    return 0;

} catch (const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
    return 1;
}
