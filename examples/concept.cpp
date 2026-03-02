#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

namespace fp = flowpp;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        using Msg = fp::data<std::string>;

        struct Printer final : fp::observer {
            void notify(std::unique_ptr<Msg> data) override {
                if (data) std::cout << "[Output] " << data->get() << '\n';
            }
        };

        struct KeyScanner final : fp::observable {
            std::unique_ptr<Msg> generate() {
                std::string token;
                if (std::cin >> token) return std::make_unique<Msg>(std::move(token));
                if (std::cin.bad()) throw std::runtime_error("stdin I/O error");
                return nullptr;
            }
        };

        fp::flowpp_engine engine;

        auto& scanner = *engine.instantiate<KeyScanner>();
        auto& counter = *engine.instantiate<fp::counter>();
        auto& printer = *engine.instantiate<Printer>();

        scanner.subscribe(&counter);
        counter.subscribe(&printer);

        engine.run();

        std::cout << "Processed Tokens: " << counter.get() << '\n';
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
