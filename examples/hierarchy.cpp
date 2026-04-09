#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>
#include <mutex>
#include <vector>

// --- Modern Thread-Safe Framework ---
namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    const uint64_t INFINITE = 0xFFFFFFFFFFFFFFFF;

    class observer { 
    public: 
        virtual ~observer() = default; 
        virtual void on_notify() = 0; // Pure virtual for concrete implementations
    };

    class observable {
    public:
        virtual ~observable() = default;

        /**
         * IMPROVEMENT: Thread-Safe Subscription.
         * Protects the internal list of observers from concurrent access 
         * if graphs run in parallel threads.
         */
        void subscribe(observer* obs) {
            if (!obs) return;
            std::scoped_lock lock(mtx);
            observers.push_back(obs);
        }

    private:
        std::mutex mtx;
        std::vector<observer*> observers;
    };

    // Concepts for C++20
    template<typename T>
    concept FlowObserver = std::derived_from<T, observer>;

    template<typename T>
    concept FlowObservable = std::derived_from<T, observable>;

    // Concrete Components
    class file_src     : public observable {};
    class file_sink    : public observer { public: void on_notify() override {} };
    class json_encoder : public observer, public observable { public: void on_notify() override {} };
    class json_decoder : public observer, public observable { public: void on_notify() override {} };
    class tcp_sender   : public observer { public: void on_notify() override {} };
    class tcp_recver   : public observable {};
}

using namespace flowpp;
using DataPtr = std::unique_ptr<data<http_msg>>;

// --- Logic-Injected Component ---
class HttpFlowContainer : public observer, public observable {
public:
    using Processor = std::function<DataPtr(const DataPtr&, uint64_t)>;

    // Rule of Zero: No manual destructor/copy logic needed
    explicit HttpFlowContainer(std::string_view contentType, Processor proc = nullptr) 
        : content_type(contentType), processor(std::move(proc)) {}

    void on_notify() override { /* Process incoming data */ }

    [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) noexcept {
        return processor ? processor(dat, timeout) : nullptr;
    }

private:
    std::string content_type;
    Processor processor;
};

// --- Universal Pipe Operators ---

// Handles Raw Pointers
template <FlowObservable T, FlowObserver U>
auto* operator|(T* lhs, U* rhs) {
    if (lhs && rhs) lhs->subscribe(rhs);
    return rhs;
}

// Handles Unique Pointers
template <FlowObservable T, FlowObserver U>
auto& operator|(const std::unique_ptr<T>& lhs, const std::unique_ptr<U>& rhs) {
    lhs.get() | rhs.get();
    return rhs;
}

// --- Main Execution ---
int main() {
    try {
        // 1. Memory Setup
        auto fileSrc   = std::make_unique<file_src>();
        auto jsonEnc   = std::make_unique<json_encoder>();
        auto tcpSend   = std::make_unique<tcp_sender>();
        auto tcpRecv   = std::make_unique<tcp_recver>();
        auto fileSink  = std::make_unique<file_sink>();

        // 2. Component with custom behavior
        auto httpCont = std::make_unique<HttpFlowContainer>("application/json", 
            [](const DataPtr& d, uint64_t t) -> DataPtr {
                return std::make_unique<data<http_msg>>();
            });

        /**
         * 3. FINAL SYNTAX:
         * Thread-safe, type-checked (C++20), and no raw pointer management.
         */
        fileSrc | jsonEnc  | httpCont | tcpSend;
        tcpRecv | httpCont | fileSink;

        std::cout << "Thread-safe pipeline initialized." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
