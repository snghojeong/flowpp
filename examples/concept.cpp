#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

namespace fp = flowpp;

int main() try {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Msg = fp::data<std::string>;

    struct Scanner final : fp::observable {
        std::unique_ptr<Msg> generate() {
            std::string s;
            if (std::cin >> s) return std::make_unique<Msg>(std::move(s));
            if (std::cin.bad()) throw std::runtime_error("stdin I/O error");
            return nullptr; // EOF
        }
    };

    struct Printer final : fp::observer {
        void notify(std::unique_ptr<Msg> d) override {
            if (d) std::cout << d->get() << '\n';
        }
    };

    fp::flowpp_engine engine;

    auto& scanner = *engine.instantiate<Scanner>();
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
