#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

namespace fp = flowpp;

namespace app {

// One obvious way to connect nodes.
inline void connect(fp::observable& src, fp::observer& dst) {
    src.subscribe(&dst);
}

// Prints the received string payload.
class Printer final : public fp::observer {
public:
    void notify(std::unique_ptr<fp::data<std::string>> msg) override {
        if (!msg) return;
        std::cout << "[Output] " << msg->get() << '\n';
    }
};

// Produces tokens from stdin until EOF.
class KeyScanner final : public fp::observable {
public:
    std::unique_ptr<fp::data<std::string>> generate() {
        std::string token;
        if (std::cin >> token) {
            return std::make_unique<fp::data<std::string>>(std::move(token));
        }

        // If extraction failed because of a real I/O error, report it.
        if (std::cin.bad()) {
            throw std::runtime_error("stdin I/O error");
        }

        // Otherwise: EOF or recoverable state => signal end-of-stream.
        return nullptr;
    }
};

void configure_fast_io() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
}

} // namespace app

int main() {
    app::configure_fast_io();

    try {
        fp::flowpp_engine engine;

        // Construct nodes.
        auto& scanner = *engine.instantiate<app::KeyScanner>();
        auto& counter = *engine.instantiate<fp::counter>();
        auto& printer = *engine.instantiate<app::Printer>();

        // Wire pipeline.
        app::connect(scanner, counter);
        app::connect(counter, printer);

        // Run.
        engine.run();

        // Report.
        std::cout << "\n--- Execution Summary ---\n"
                  << "Processed Tokens: " << counter.get() << '\n';

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
