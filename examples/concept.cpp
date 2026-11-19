
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

// Replace with the actual framework header.
// #include <flowpp/flowpp.hpp>

namespace fp = flowpp;

namespace {

using Message = fp::data<std::string>;

// Centralized output validation.
void check_output(std::ostream& output) {
    if (!output) {
        throw std::ios_base::failure("Output operation failed");
    }
}

class Scanner final : public fp::observable {
public:
    explicit Scanner(std::istream& input)
        : input_(input) {}

    [[nodiscard]]
    std::unique_ptr<Message> generate() override {
        std::string token;

        try {
            if (input_ >> token) {
                return std::make_unique<Message>(
                    std::move(token)
                );
            }
        } catch (const std::ios_base::failure&) {
            // Normal EOF may throw if failbit exceptions
            // are enabled on the input stream.
            if (input_.bad() || !input_.eof()) {
                throw;
            }
            return nullptr;
        }

        if (input_.bad() || !input_.eof()) {
            throw std::ios_base::failure("Input read failed");
        }

        return nullptr;
    }

private:
    std::istream& input_;
};

class Printer final : public fp::observer {
public:
    explicit Printer(std::ostream& output)
        : output_(output) {}

    void notify(const Message& message) override {
        output_ << message.get() << '\n';
        check_output(output_);
    }

private:
    std::ostream& output_;
};

template <typename Count>
void print_summary(std::ostream& diagnostics, const Count& count) {
    diagnostics
        << "\n--- Execution Summary ---\n"
        << "Tokens Processed: " << count << '\n'
        << "Status: Success\n";

    diagnostics.flush();
    check_output(diagnostics);
}

void run(
    std::istream& input,
    std::ostream& output,
    std::ostream& diagnostics
) {
    if (!input.good() || !output.good() || !diagnostics.good()) {
        throw std::ios_base::failure("Stream is not ready");
    }

    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>(input);
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>(output);

    scanner->subscribe(counter);
    counter->subscribe(printer);

    engine.run();

    output.flush();
    check_output(output);

    print_summary(diagnostics, counter->get());
}

} // namespace

int main() {
    try {
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);

        run(std::cin, std::cout, std::cerr);
        return EXIT_SUCCESS;

    } catch (const std::exception& e) {
        std::cerr << "Application error: "
                  << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
