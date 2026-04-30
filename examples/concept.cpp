#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cstdlib>

// Assuming flowpp is a placeholder for a reactive framework
namespace fp = flowpp;

int main() try {
    // Optimization: Standard Fast I/O setup
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Msg = fp::data<std::string>;
    using MsgPtr = std::unique_ptr<Msg>;

    // IMPROVEMENT: Moving logic into a cohesive internal structure 
    // or using 'final' to allow the compiler to devirtualize calls.
    struct Scanner final : fp::observable {
        [[nodiscard]] MsgPtr generate() override {
            std::string s;
            // Combined skip-whitespace and extraction
            if (!(std::cin >> std::ws >> s)) {
                if (std::cin.bad()) throw std::runtime_error("Fatal stdin I/O error");
                return nullptr; // EOF/Termination signal
            }
            return std::make_unique<Msg>(std::move(s));
        }
    };

    struct Printer final : fp::observer {
        // IMPROVEMENT: Using 'const' and 'noexcept' consistently 
        // to ensure the pipeline never breaks during the sink phase.
        void notify(const Msg& d) noexcept override {
            std::cout << d.get() << '\n';
        }
    };

    fp::flowpp_engine engine;

    // Component instantiation
    auto scanner = engine.instantiate<Scanner>();
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>();

    // Pipeline setup: Scanner -> Counter -> Printer
    // IMPROVEMENT: If the framework supports it, chain these. 
    // Otherwise, this explicit subscription is clear and safe.
    scanner->subscribe(counter);
    counter->subscribe(printer);

    // Start the reactive processing loop
    engine.run();

    // Final Report Management
    // IMPROVEMENT: Use a single '\n' before the summary to ensure 
    // a clear visual break from the streamed data.
    std::cout << "\n--- Execution Summary ---\n"
              << "Tokens Processed: " << counter->get() << "\n"
              << "Status: Success" << std::endl;
    
    return EXIT_SUCCESS;

} catch (const std::exception& e) {
    // Use std::endl here to force flush during failure
    std::cerr << "Application Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
