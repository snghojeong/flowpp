
#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

// Replace with the actual flowpp framework header.
// #include <flowpp/flowpp.hpp>

namespace fp = flowpp;

namespace {

using Message = fp::data<std::string>;
using MessagePtr = std::unique_ptr<Message>;

// ------------------------------------------------------------
// Input source
// ------------------------------------------------------------

class Scanner final : public fp::observable {
public:
    explicit Scanner(std::istream& input) noexcept
        : input_(input) {}

    [[nodiscard]] MessagePtr generate() override {
        switch (state_) {
            case State::Completed:
                return nullptr;

            case State::Failed:
                throw std::logic_error(
                    "Scanner is in a failed state"
                );

            case State::Active:
                break;
        }

        std::string token;

        try {
            if (input_ >> token) {
                return std::make_unique<Message>(
                    std::move(token)
                );
            }

            if (input_.bad()) {
                throw std::ios_base::failure(
                    "Fatal input I/O error"
                );
            }

            if (input_.eof()) {
                state_ = State::Completed;
                return nullptr;
            }

            throw std::ios_base::failure(
                "Unexpected input extraction failure"
            );

        } catch (const std::ios_base::failure&) {
            // failbit exceptions can occur at normal EOF.
            if (!input_.bad() && input_.eof()) {
                state_ = State::Completed;
                return nullptr;
            }

            state_ = State::Failed;
            throw;

        } catch (...) {
            // Preserve failure state for all other errors,
            // including allocation and construction failures.
            state_ = State::Failed;
            throw;
        }
    }

private:
    enum class State {
        Active,
        Completed,
        Failed
    };

    std::istream& input_;
    State state_ = State::Active;
};

// ------------------------------------------------------------
// Output sink
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// Output helpers
// ------------------------------------------------------------

void flush_output(std::ostream& output) {
    output.flush();

    if (!output) {
        throw std::ios_base::failure(
            "Failed to flush output"
        );
    }
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

// ------------------------------------------------------------
// Stream validation
// ------------------------------------------------------------

void validate_streams(
    std::istream& input,
    std::ostream& output,
    std::ostream& diagnostics
) {
    const auto* input_stream =
        static_cast<const std::ios*>(&input);

    const auto* output_stream =
        static_cast<const std::ios*>(&output);

    const auto* diagnostics_stream =
        static_cast<const std::ios*>(&diagnostics);

    if (input_stream == output_stream ||
        input_stream == diagnostics_stream ||
        output_stream == diagnostics_stream) {
        throw std::invalid_argument(
            "Input, output, and diagnostic streams "
            "must be distinct stream objects"
        );
    }

    if (!input.good()) {
        throw std::ios_base::failure(
            "Input stream is not ready for reading"
        );
    }

    if (!output.good()) {
        throw std::ios_base::failure(
            "Output stream is not ready for writing"
        );
    }

    if (!diagnostics.good()) {
        throw std::ios_base::failure(
            "Diagnostic stream is not ready for writing"
        );
    }
}

// ------------------------------------------------------------
// Reactive pipeline
// ------------------------------------------------------------

int run(
    std::istream& input,
    std::ostream& output,
    std::ostream& diagnostics
) {
    validate_streams(input, output, diagnostics);

    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>(input);
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>(output);

    // Scanner -> Counter -> Printer
    scanner->subscribe(counter);
    counter->subscribe(printer);

    engine.run();

    // Flush processed data before reporting success.
    flush_output(output);

    // Report statistics separately from application data.
    print_summary(diagnostics, counter->get());

    return EXIT_SUCCESS;
}

} // namespace

// ------------------------------------------------------------
// Application entry point
// ------------------------------------------------------------

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
