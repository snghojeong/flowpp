
#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

// Replace with the actual flowpp framework header.
// #include <flowpp/flowpp.hpp>

namespace fp = flowpp;

namespace {

using Message = fp::data<std::string>;
using MessagePtr = std::unique_ptr<Message>;

class Scanner final : public fp::observable {
public:
    explicit Scanner(std::istream& input) noexcept
        : input_(input) {}

    [[nodiscard]] MessagePtr generate() override {
        std::string token;

        if (input_ >> token) {
            return std::make_unique<Message>(
                std::move(token)
            );
        }

        // Physical I/O errors take precedence over EOF.
        if (input_.bad()) {
            throw std::ios_base::failure(
                "Fatal input I/O error"
            );
        }

        if (input_.eof()) {
            return nullptr;
        }

        throw std::ios_base::failure(
            "Unexpected input extraction failure"
        );
    }

private:
    std::istream& input_;
};

class Printer final : public fp::observer {
public:
    explicit Printer(std::ostream& output) noexcept
        : output_(output) {}

    void notify(const Message& message) override {
        output_ << message.get() << '\n';

        if (!output_) {
            throw std::ios_base::failure(
                "Failed to write output"
            );
        }
    }

private:
    std::ostream& output_;
};

void configure_io() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
}

template <typename Count>
void print_summary(
    std::ostream& diagnostics,
    const Count& count
) {
    diagnostics
        << "\n--- Execution Summary ---\n"
        << "Tokens Processed: " << count << '\n'
        << "Status: Success\n";

    diagnostics.flush();

    if (!diagnostics) {
        throw std::ios_base::failure(
            "Failed to write execution summary"
        );
    }
}

void flush_output(std::ostream& output) {
    output.flush();

    if (!output) {
        throw std::ios_base::failure(
            "Failed to flush output"
        );
    }
}

int run(
    std::istream& input,
    std::ostream& output,
    std::ostream& diagnostics
) {
    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>(input);
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>(output);

    // Scanner -> Counter -> Printer
    scanner->subscribe(counter);
    counter->subscribe(printer);

    // Process messages until EOF.
    engine.run();

    // Ensure processed data is flushed before success.
    flush_output(output);

    // Keep diagnostics separate from application data.
    print_summary(diagnostics, counter->get());

    return EXIT_SUCCESS;
}

} // namespace

int main() {
    try {
        configure_io();

        return run(
            std::cin,
            std::cout,
            std::cerr
        );
    } catch (const std::exception& e) {
        std::cerr
            << "Application error: "
            << e.what() << '\n';

        return EXIT_FAILURE;
    } catch (...) {
        std::cerr
            << "Unknown application error\n";

        return EXIT_FAILURE;
    }
}
