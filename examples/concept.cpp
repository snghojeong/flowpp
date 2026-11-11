
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
// Transactional RAII stream exception-mask guard
// ------------------------------------------------------------

class StreamExceptionGuard final {
public:
    StreamExceptionGuard(
        std::ios& stream,
        std::ios::iostate exception_mask
    )
        : stream_(stream),
          original_mask_(stream.exceptions()) {
        try {
            stream_.exceptions(exception_mask);
        } catch (...) {
            // Restore the original exception mask if
            // configuration fails during construction.
            restore();
            throw;
        }
    }

    ~StreamExceptionGuard() noexcept {
        restore();
    }

    StreamExceptionGuard(
        const StreamExceptionGuard&
    ) = delete;

    StreamExceptionGuard& operator=(
        const StreamExceptionGuard&
    ) = delete;

    StreamExceptionGuard(
        StreamExceptionGuard&&
    ) = delete;

    StreamExceptionGuard& operator=(
        StreamExceptionGuard&&
    ) = delete;

private:
    void restore() noexcept {
        try {
            stream_.exceptions(original_mask_);
        } catch (...) {
            // exceptions(mask) updates the mask before
            // potentially throwing because of rdstate().
            // Never propagate from cleanup.
        }
    }

    std::ios& stream_;
    std::ios::iostate original_mask_;
};

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
}

// ------------------------------------------------------------
// Source: whitespace-delimited token scanner
// ------------------------------------------------------------

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

        // Normal EOF terminates the source.
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

// ------------------------------------------------------------
// Sink: processed message printer
// ------------------------------------------------------------

class Printer final : public fp::observer {
public:
    explicit Printer(std::ostream& output) noexcept
        : output_(output) {}

    void notify(const Message& message) override {
        output_ << message.get() << '\n';
    }

private:
    std::ostream& output_;
};

// ------------------------------------------------------------
// Execution summary
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// Reactive pipeline
// ------------------------------------------------------------

int run(
    std::istream& input,
    std::ostream& output,
    std::ostream& diagnostics
) {
    // Reject conflicting stream configurations.
    validate_streams(input, output, diagnostics);

    // Each guard restores its original mask on exit.
    // Construction is also exception-safe.
    StreamExceptionGuard input_guard(
        input,
        std::ios::badbit
    );

    StreamExceptionGuard output_guard(
        output,
        std::ios::badbit | std::ios::failbit
    );

    StreamExceptionGuard diagnostics_guard(
        diagnostics,
        std::ios::badbit | std::ios::failbit
    );

    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>(input);
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>(output);

    // Scanner -> Counter -> Printer
    scanner->subscribe(counter);
    counter->subscribe(printer);

    // Execute until Scanner signals EOF.
    engine.run();

    // Flush processed data before reporting success.
    output.flush();

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
