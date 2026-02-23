#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace fp = flowpp;

/**
 * Simplified Printer: Focuses only on the core action.
 * Using template specialization or auto (C++20) can make this more flexible.
 */
template <typename T>
class Printer final : public fp::observer {
public:
    void notify(std::unique_ptr<fp::data<T>> data) override {
        if (data) {
            std::cout << "[Output] " << data->get() << std::endl;
        }
    }
};

/**
 * KeyScanner: Streamlines the cin extraction logic.
 */
class KeyScanner final : public fp::observable {
public:
    auto generate() -> std::unique_ptr<fp::data<std::string>> {
        std::string token;
        
        if (std::cin >> token) {
            return std::make_unique<fp::data<std::string>>(std::move(token));
        }

        if (std::cin.bad()) {
            throw std::runtime_error("KeyScanner: Unrecoverable stream error");
        }

        return nullptr; // Clean EOF
    }
};

// --- Pipeline Helper ---

/**
 * Connects multiple components in a chain: connect(a, b, c) -> a->b->c
 */
template <typename Src, typename... Dsts>
void connect_chain(Src& source, Dsts&... destinations) {
    fp::observable* current = &source;
    auto link = [&](auto* next) {
        current->subscribe(next);
        if constexpr (std::is_base_of_v<fp::observable, std::decay_t<decltype(*next)>>) {
            current = next;
        }
    };
    (link(&destinations), ...);
}

// --- Main ---

int main() {
    // Optimize I/O
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        fp::flowpp_engine engine;

        // Instantiate components
        auto scanner = engine.instantiate<KeyScanner>();
        auto counter = engine.instantiate<fp::counter>(); 
        auto printer = engine.instantiate<Printer<std::string>>();

        // Descriptive wiring: Scanner -> Counter -> Printer
        connect_chain(*scanner, *counter, *printer);

        // Run engine
        engine.run(1000, 1000);

        std::cout << "\n--- Stats ---\n"
                  << "Total Tokens: " << counter->get() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
