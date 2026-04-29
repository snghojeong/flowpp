#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cstdlib>
#include <iomanip> // For potential I/O formatting

// Assuming flowpp is a placeholder for a reactive framework
namespace fp = flowpp;

int main() try {
    // Optimization: Standard Fast I/O setup
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Msg = fp::data<std::string>;
    using MsgPtr = std::unique_ptr<Msg>;

    struct Scanner final : fp::observable {
        [[nodiscard]] MsgPtr generate() {
            std::string s;
            // IMPROVEMENT: Use std::ws to skip leading whitespace efficiently 
            // before attempting to extract the next token.
            if (!(std::cin >> std::ws >> s)) {
                if (std::cin.bad()) throw std::runtime_error("stdin I/O error");
                return nullptr; // EOF
            }
            return std::make_unique<Msg>(std::move(s));
        }
    };

    struct Printer final : fp::observer {
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

    // Pipeline setup: Scanner -> Counter -> Printer
    scanner->subscribe(counter);
    counter->subscribe(printer);

    // Execution phase
    engine.run();

    // IMPROVEMENT: Final flush and clean reporting. 
    // We use a separator for visual clarity in terminal output.
    std::cout << std::flush;
    std::cout << "--------------------------\n";
    std::cout << "Tokens Processed: " << counter->get() << "\n";
    std::cout << "--------------------------" << std::endl;
    
    return EXIT_SUCCESS;

} catch (const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
