#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

namespace fp = flowpp;

class Printer final : public fp::observer {
public:
    void notify(std::unique_ptr<fp::data<std::string>> data) override {
        if (data) std::cout << "[Output] " << data->get() << '\n';
    }
};

class KeyScanner final : public fp::observable {
public:
    std::unique_ptr<fp::data<std::string>> generate() {
        std::string token;
        if (std::cin >> token)
            return std::make_unique<fp::data<std::string>>(std::move(token));

        if (std::cin.bad())
            throw std::runtime_error("stdin I/O error");

        return nullptr; // EOF / stop
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

        scanner.subscribe(&counter);
        counter.subscribe(&printer);

        engine.run();

        std::cout << "\n--- Execution Summary ---\n"
                  << "Processed Tokens: " << counter.get() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
