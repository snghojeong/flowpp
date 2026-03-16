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
            // Directly constructing the unique_ptr from the stream result
            if (std::cin >> s) return std::make_unique<Msg>(std::move(s));
            if (std::cin.bad()) throw std::runtime_error("stdin I/O error");
            return nullptr; // EOF signal
        }
    };

    struct Printer final : fp::observer {
        // IMPROVEMENT: Accept by const reference to avoid ownership transfer
        // and allow the engine to broadcast the same message to other observers.
        void notify(const Msg& d) override {
            std::cout << d.get() << '\n';
        }
    };

    fp::flowpp_engine engine;

    // Standard engine instantiation
    auto& scanner = *engine.instantiate<Scanner>();
    auto& counter = *engine.instantiate<fp::counter>();
    auto& printer = *engine.instantiate<Printer>();

    // Establishing the pipeline
    scanner.subscribe(&counter);
    counter.subscribe(&printer);

    engine.run();

    std::cout << "Processed Tokens: " << counter.get() << '\n';
    return 0;

} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
}
