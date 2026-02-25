#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

namespace fp = flowpp;

// Overload pipe operator for clean DSL-style wiring
template <typename Src, typename Dst>
auto& operator|(Src& source, Dst& destination) {
    source.subscribe(&destination);
    return destination;
}

/**
 * Generic Printer: Simplifies data output to console.
 */
class Printer final : public fp::observer {
public:
    void notify(std::unique_ptr<fp::data<std::string>> data) override {
        if (data) std::cout << "[Output] " << data->get() << "\n";
    }
};

/**
 * KeyScanner: Handles input tokenization with basic error checking.
 */
class KeyScanner final : public fp::observable {
public:
    auto generate() -> std::unique_ptr<fp::data<std::string>> {
        std::string token;
        if (std::cin >> token) {
            return std::make_unique<fp::data<std::string>>(std::move(token));
        }
        if (std::cin.bad()) throw std::runtime_error("Critical I/O failure");
        return nullptr; 
    }
};

int main() {
    // Optimization for high-throughput stream processing
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        fp::flowpp_engine engine;

        // Instantiate components directly through the engine
        auto& scanner = *engine.instantiate<KeyScanner>();
        auto& counter = *engine.instantiate<fp::counter>(); 
        auto& printer = *engine.instantiate<Printer>();

        // Declarative pipeline definition
        scanner | counter | printer;

        engine.run(); // Standard run without hardcoded limits unless necessary

        std::cout << "\n--- Execution Summary ---\n"
                  << "Processed Tokens: " << counter.get() << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
