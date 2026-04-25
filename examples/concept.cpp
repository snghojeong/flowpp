#include <iostream>
#include <memory>
#include <string>
#include <string_view> // IMPROVEMENT: For efficient, non-owning string access
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
        // IMPROVEMENT: Use std::string_view if the Msg::get() allows it,
        // or simply ensure we treat the result as a view to prevent copies.
        void notify(const Msg& d) noexcept override {
            const std::string_view view = d.get(); 
            std::cout << view << '\n';
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

    // Run the reactive engine
    engine.run();

    // Final output management
    std::cout << std::flush;
    std::cout << "--- Processed Tokens: " << counter->get() << " ---\n";
    
    return 0;

} catch (const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
    return 1;
}
