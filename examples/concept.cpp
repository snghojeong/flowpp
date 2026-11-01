
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

// Include the actual flowpp framework header.
// #include <flowpp/flowpp.hpp>

namespace fp = flowpp;

namespace {

using Message = fp::data<std::string>;
using MessagePtr = std::unique_ptr<Message>;

class Scanner final : public fp::observable {
public:
    [[nodiscard]] MessagePtr generate() override {
        std::string token;

        if (!(std::cin >> token)) {
            if (std::cin.bad()) {
                throw std::ios_base::failure(
                    "Failed to read standard input"
                );
            }

            // EOF or extraction failure terminates the source.
            return nullptr;
        }

        return std::make_unique<Message>(std::move(token));
    }
};

class Printer final : public fp::observer {
public:
    void notify(const Message& message) override {
        std::cout << message.get() << '\n';

        if (!std::cout) {
            throw std::ios_base::failure(
                "Failed to write standard output"
            );
        }
    }
};

void configure_io() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
}

void print_summary(std::size_t count) {
    std::cout
        << "\n--- Execution Summary ---\n"
        << "Tokens Processed: " << count << '\n'
        << "Status: Success\n";

    std::cout.flush();

    if (!std::cout) {
        throw std::ios_base::failure(
            "Failed to write execution summary"
        );
    }
}

int run() {
    configure_io();

    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>();
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>();

    scanner->subscribe(counter);
    counter->subscribe(printer);

    engine.run();

    print_summary(counter->get());

    return EXIT_SUCCESS;
}

} // namespace

int main() {
    try {
        return run();
    } catch (const std::exception& e) {
        std::cerr << "Application error: "
                  << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown application error\n";
        return EXIT_FAILURE;
    }
}
