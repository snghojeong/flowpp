
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

// Replace with the actual flowpp header.
// #include <flowpp/flowpp.hpp>

namespace fp = flowpp;

namespace {

using Message = fp::data<std::string>;

class Scanner final : public fp::observable {
public:
    explicit Scanner(std::istream& input)
        : input_(input) {}

    [[nodiscard]]
    std::unique_ptr<Message> generate() override {
        std::string token;

        if (input_ >> token) {
            return std::make_unique<Message>(
                std::move(token)
            );
        }

        if (input_.bad() || !input_.eof()) {
            throw std::ios_base::failure("Input failed");
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
    }

private:
    std::ostream& output_;
};

void run(
    std::istream& input,
    std::ostream& output,
    std::ostream& diagnostics
) {
    if (!input.good() || !output.good() || !diagnostics.good()) {
        throw std::ios_base::failure("Stream is not ready");
    }

    // Output errors throw automatically.
    output.exceptions(std::ios::badbit | std::ios::failbit);
    diagnostics.exceptions(std::ios::badbit | std::ios::failbit);

    fp::flowpp_engine engine;

    auto scanner = engine.instantiate<Scanner>(input);
    auto counter = engine.instantiate<fp::counter>();
    auto printer = engine.instantiate<Printer>(output);

    scanner->subscribe(counter);
    counter->subscribe(printer);

    engine.run();

    output.flush();

    diagnostics
        << "\n--- Execution Summary ---\n"
        << "Tokens Processed: " << counter->get() << '\n'
        << "Status: Success\n";

    diagnostics.flush();
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
