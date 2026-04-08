#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>

// --- Modern Framework Definitions ---
namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    const uint64_t INFINITE = 0xFFFFFFFFFFFFFFFF;

    // Interface definitions
    class observer { public: virtual ~observer() = default; };
    class observable {
    public:
        virtual ~observable() = default;
        void subscribe(observer* obs) { if(obs) /* Internal link */ ; }
    };

    // Concepts for compile-time safety (C++20)
    template<typename T>
    concept FlowObserver = std::derived_from<T, observer>;

    template<typename T>
    concept FlowObservable = std::derived_from<T, observable>;

    // Concrete Components
    class file_src : public observable {};
    class file_sink : public observer {};
    class json_encoder : public observer, public observable {};
    class json_decoder : public observer, public observable {};
    class tcp_sender : public observer {};
    class tcp_recver : public observable {};
}

using namespace flowpp;
using DataPtr = std::unique_ptr<data<http_msg>>;

// --- Improved Component with Dependency Injection ---
class HttpFlowContainer : public observer, public observable {
public:
    // IMPROVEMENT: Inject logic via std::function to make the container reusable
    using Processor = std::function<DataPtr(const DataPtr&, uint64_t)>;

    explicit HttpFlowContainer(std::string_view contentType, Processor proc = nullptr) 
        : processor(std::move(proc)) {}

    [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) noexcept {
        if (processor) return processor(dat, timeout);
        return dat ? std::make_unique<data<http_msg>>() : nullptr;
    }

private:
    Processor processor;
};

// --- Modern Pipe Operators ---

/**
 * IMPROVEMENT: Using C++20 Concepts.
 * This ensures the pipe operator only works on types that satisfy 
 * the FlowObservable and FlowObserver requirements.
 */
template <FlowObservable T, FlowObserver U>
auto* operator|(T* lhs, U* rhs) {
    if (lhs && rhs) lhs->subscribe(rhs);
    return rhs;
}

// Support for chaining mixed types (unique_ptr and raw pointers)
template <FlowObservable T, FlowObserver U>
auto& operator|(const std::unique_ptr<T>& lhs, const std::unique_ptr<U>& rhs) {
    lhs.get() | rhs.get();
    return rhs;
}

// --- Main Execution ---
int main() {
    try {
        // Use smart pointers for root-level ownership
        auto fileSrc   = std::make_unique<file_src>();
        auto jsonEnc   = std::make_unique<json_encoder>();
        auto tcpSend   = std::make_unique<tcp_sender>();
        
        // Custom logic injected into the container at runtime
        auto httpCont = std::make_unique<HttpFlowContainer>("application/json", 
            [](const DataPtr& d, uint64_t t) -> DataPtr {
                std::cout << "Custom processing logic triggered..." << std::endl;
                return std::make_unique<data<http_msg>>();
            });

        /**
         * FINAL REFINED SYNTAX:
         * Clean, type-safe, and highly readable.
         */
        fileSrc | jsonEnc | httpCont | tcpSend;

        std::cout << "Pipeline fully operational with injected logic." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal Pipeline Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
