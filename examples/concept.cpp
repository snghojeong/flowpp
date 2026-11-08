
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

        // badbit automatically throws when enabled.
        if (input_ >> token) {
            return std::make_unique<Message>(
                std::move(token)
            );
        }

        // EOF is the expected termination condition.
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
        // failbit/badbit automatically throw.
        output_ << message.get() << '\n';
    }

private:
    std::ostream& output_;
};

void configure_io(
    std::istream& input,
    std::ostream& output,
    std::ostream& diagnostics
) {
    // EOF sets failbit during formatted extraction,
    // so input must not throw on failbit.
    input.exceptions(std::ios::badbit);

    // Output failures should be reported immediately.
    output.exceptions(
        std::ios::badbit | std::ios::failbit
    );

    diagnostics.exceptions(
        std::ios::badbit | std::ios::failbit
    );
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
}

int run(
    std::istream& input,
    std::ostream& output,
    std::ostream& diagnostics
) {
    configure_io(input, output, diagnostics);

    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>(input);
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>(output);

    // Scanner -> Counter -> Printer
    scanner->subscribe(counter);
    counter->subscribe(printer);

    // Process messages until EOF.
    engine.run();

    // Ensure output is flushed before reporting success.
    output.flush();

    print_summary(diagnostics, counter->get());

    return EXIT_SUCCESS;
}

} // namespace

int main() {
    try {
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);

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
