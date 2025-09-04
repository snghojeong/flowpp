#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// Assumptions:
//   - flowpp provides: flowpp::observable, flowpp::observer,
//     flowpp::data<T>, flowpp::flowpp_engine, flowpp::counter.
//   - engine.instantiate<T>() returns std::unique_ptr<T>.
// #include "flowpp_headers.h"

namespace fp = flowpp;  // Avoids global 'using namespace'.

// -----------------------------
// Observers / Sources
// -----------------------------

// Prints incoming data<T> to stdout.
template <typename T>
class Printer final : public fp::observer {
    using DataPtr = std::unique_ptr<fp::data<T>>;
public:
    void notify(DataPtr dat) override {
        if (dat) {
            std::cout << "Printed: " << dat->get() << '\n';
        }
    }
};

// Reads whitespace-delimited tokens from stdin and emits them as data<std::string>.
class KeyScanner final : public fp::observable {
public:
    using DataPtr = std::unique_ptr<fp::data<std::string>>;

    // Returns nullptr on clean EOF to signal end-of-stream.
    [[nodiscard]] DataPtr generate() {
        std::string token;
        if (std::cin >> token) {
            return std::make_unique<fp::data<std::string>>(token);
        }
        if (std::cin.eof()) {
            return nullptr;  // graceful EOS
        }
        throw std::runtime_error("KeyScanner: input stream error");
    }
};

// -----------------------------
// Wiring helper
// -----------------------------

// Intention-revealing connector instead of overloading '|'.
inline fp::observable& connect(fp::observable& src, fp::observer& dst) {
    src.subscribe(&dst);
    return src;
}

// If you like the pipe style, keep it—but make it explicit and local.
inline fp::observable& operator|(fp::observable& lhs, fp::observer& rhs) {
    return connect(lhs, rhs);
}

// -----------------------------
// Main
// -----------------------------
int main() {
    try {
        constexpr int kTickMs    = 1'000;  // engine tick timeout (ms)
        constexpr int kMaxTicks  = 1'000;  // engine loop iterations

        // Engine + components
        auto engine  = std::make_unique<fp::flowpp_engine>();
        auto scanner = engine->instantiate<KeyScanner>();
        auto counter = engine->instantiate<fp::counter>();              // assumes counter is both observer+observable
        auto printer = engine->instantiate<Printer<std::string>>();

        // Pipeline: stdin -> counter -> printer
        *scanner | *counter;
        *counter | *printer;

        // Run loop
        engine->run(kTickMs, kMaxTicks);

        // Final report
        std::cout << "\n--- Final Results ---\n";
        std::cout << "Key count: " << counter->get() << '\n';
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << '\n';
        return 1;
    }
}
