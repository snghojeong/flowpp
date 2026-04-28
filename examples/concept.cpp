#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cstdlib> // IMPROVEMENT: For EXIT_FAILURE and EXIT_SUCCESS

// Assuming flowpp is a placeholder for a reactive framework
namespace fp = flowpp;

int main() try {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Msg = fp::data<std::string>;
    using MsgPtr = std::unique_ptr<Msg>;

    struct Scanner final : fp::observable {
        [[nodiscard]] MsgPtr generate() {
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
        void notify(const Msg& d) noexcept override {
            const std::string_view view = d.get(); 
            std::cout << view << '\n';
        }
    };

    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>();
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>();

    scanner->subscribe(counter);
    counter->subscribe(printer);

    engine.run();

    std::cout << std::flush;
    std::cout << "--- Processed Tokens: " << counter->get() << " ---\n";
    
    return EXIT_SUCCESS; // IMPROVEMENT: Semantic return code

} catch (const std::exception& e) {
    // IMPROVEMENT: Using std::endl here is appropriate to force a flush 
    // during a critical failure so the user sees the error before termination.
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return EXIT_FAILURE; // IMPROVEMENT: Standardized error signaling
}
