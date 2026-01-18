#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <ios>
#include <utility>

// Assumptions:
//   - flowpp provides: flowpp::observable, flowpp::observer,
//     flowpp::data<T>, flowpp::flowpp_engine, flowpp::counter.
//   - engine.instantiate<T>() returns std::unique_ptr<T>.

namespace fp = flowpp;

// -----------------------------
// Observers / Sources
// -----------------------------

// Prints incoming data<T> to stdout.
template <typename T>
class Printer final : public fp::observer {
public:
    using DataPtr = std::unique_ptr<fp::data<T>>;

    void notify(DataPtr dat) override {
        if (!dat) return;            // ignore EOS or null events
        std::cout << "Printed: " << dat->get() << '\n';
    }
};

// Reads whitespace-delimited tokens from stdin and emits them as data<std::string>.
class KeyScanner final : public fp::observable {
public:
    using DataPtr = std::unique_ptr<fp::data<std::string>>;

    [[nodiscard]] DataPtr generate() {
        std::string token;
        if (std::cin >> token) {
            return std::make_unique<fp::data<std::string>>(std::move(token));
        }

        // At this point extraction failed.
        if (std::cin.eof()) {
            return nullptr; // Clean EOF => graceful end-of-stream
        }

        // Fail without EOF => real error.
        throw std::runtime_error("KeyScanner: input stream error (not EOF)");
    }
};

// -----------------------------
// Wiring helper
// -----------------------------

// Intention-revealing connector instead of overloading operators globally.
inline void connect(fp::observable& src, fp::observer& dst) {
    src.subscribe(&dst);
}

// -----------------------------
// Main
// -----------------------------
int main() {
    // (Optional) Better interactive performance.
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        constexpr int kTickMs   = 1'000; // engine tick timeout (ms)
        constexpr int kMaxTicks = 1'000; // engine loop iterations

        fp::flowpp_engine engine;

        auto scanner = engine.instantiate<KeyScanner>();
        auto counter = engine.instantiate<fp::counter>(); // counter is observer+observable
        auto printer = engine.instantiate<Printer<std::string>>();

        // Pipeline: stdin -> counter -> printer
        connect(*scanner, *counter);
        connect(*counter, *printer);

        engine.run(kTickMs, kMaxTicks);

        std::cout << "\n--- Final Results ---\n";
        std::cout << "Key count: " << counter->get() << '\n';
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << '\n';
        return 1;
    }
}
