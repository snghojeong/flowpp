#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

namespace fp = flowpp;

/**
 * Generic Printer: Uses std::string by default for simplicity.
 */
template <typename T = std::string>
class Printer final : public fp::observer {
public:
    void notify(std::unique_ptr<fp::data<T>> data) override {
        if (data) {
            std::cout << "[Output] " << data->get() << "\n";
        }
    }
};

/**
 * KeyScanner: Encapsulates token extraction.
 */
class KeyScanner final : public fp::observable {
public:
    auto generate() -> std::unique_ptr<fp::data<std::string>> {
        std::string token;
        if (!(std::cin >> token)) {
            if (std::cin.bad()) throw std::runtime_error("I/O failure");
            return nullptr; 
        }
        return std::make_unique<fp::data<std::string>>(std::move(token));
    }
};

/**
 * Pipeline Operator: Overloading the pipe operator (|) 
 * provides a much more readable "Flow" syntax.
 */
template <typename Src, typename Dst>
auto& operator|(Src& source, Dst& destination) {
    source.subscribe(&destination);
    return destination;
}

int main() {
    // Fast I/O
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        fp::flowpp_engine engine;

        // 1. Instantiate
        auto& scanner = *engine.instantiate<KeyScanner>();
        auto& counter = *engine.instantiate<fp::counter>(); 
        auto& printer = *engine.instantiate<Printer<>>();

        // 2. Wire the pipeline using the pipe operator
        // Visual flow: Scanner -> Counter -> Printer
        scanner | counter | printer;

        // 3. Execute
        engine.run(1000, 1000);

        std::cout << "\n--- Statistics ---\n"
                  << "Total Tokens: " << counter.get() << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
