#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

namespace fp = flowpp;

// Explicit, readable wiring helper
template <class Src, class Dst>
void connect(Src& src, Dst& dst) {
    src.subscribe(&dst);
}

// Prints incoming strings
class Printer final : public fp::observer {
public:
    void notify(std::unique_ptr<fp::data<std::string>> data) override {
        if (data) std::cout << "[Output] " << data->get() << '\n';
    }
};

// Reads whitespace-delimited tokens from stdin
class KeyScanner final : public fp::observable {
public:
    std::unique_ptr<fp::data<std::string>> generate() {
        std::string token;
        if (std::cin >> token) {
            return std::make_unique<fp::data<std::string>>(std::move(token));
        }
        if (std::cin.bad()) {
            throw std::runtime_error("Critical I/O failure");
        }
        return nullptr; // EOF or non-fatal failure ends the stream
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        fp::flowpp_engine engine;

        auto& scanner = *engine.instantiate<KeyScanner>();
        auto& counter = *engine.instantiate<fp::counter>();
        auto& printer = *engine.instantiate<Printer>();

        connect(scanner, counter);
        connect(counter, printer);

        engine.run();

        std::cout << "\n--- Execution Summary ---\n"
                  << "Processed Tokens: " << counter.get() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
