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
            // Filtering at the source: skip empty/whitespace tokens
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
        // notify remains non-const because it represents an 'action' 
        // that conceptually modifies the state of the observer/output.
        void notify(const Msg& d) override {
            std::cout << d.get() << "\n";
        }
    };

    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>();
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>();

    scanner->subscribe(counter);
    counter->subscribe(printer);

    engine.run();

    // IMPROVEMENT: Ensuring we access data via a clear, read-only 
    // interface. In a real fp::counter, get() should be: 
    // size_t get() const { return count_; }
    std::cout << "--- Processed Tokens: " << counter->get() << " ---\n";
    
    return 0;

} catch (const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
    return 1;
}
