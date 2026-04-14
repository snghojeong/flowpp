#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>
#include <mutex>
#include <vector>
#include <utility>

namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    const uint64_t INFINITE = 0xFFFFFFFFFFFFFFFF;

    // --- Base Interfaces ---
    class observer { 
    public: 
        virtual ~observer() = default; 
        virtual void on_notify() = 0; 
    };

    class observable {
    public:
        virtual ~observable() = default;
        
        // Thread-safe subscription
        void subscribe(observer* obs) noexcept {
            if (!obs) return;
            try {
                std::scoped_lock lock(mtx);
                observers.push_back(obs);
            } catch (...) { /* Silent failure to maintain noexcept */ }
        }

    protected:
        void notify_observers() {
            std::scoped_lock lock(mtx);
            for (auto* obs : observers) {
                if (obs) obs->on_notify();
            }
        }

    private:
        std::mutex mtx;
        std::vector<observer*> observers;
    };

    // --- C++20 Concepts ---
    template<typename T>
    concept FlowObserver = std::derived_from<T, observer>;

    template<typename T>
    concept FlowObservable = std::derived_from<T, observable>;

    // --- The Universal Pipe Operators ---
    
    // 1. Raw Reference Pipe (noexcept for performance)
    template <FlowObservable T, FlowObserver U>
    auto& operator|(T& lhs, U& rhs) noexcept {
        lhs.subscribe(&rhs);
        return rhs;
    }

    // 2. Shared Pointer Pipe (nodiscard to prevent unused chains)
    template <FlowObservable T, FlowObserver U>
    [[nodiscard]] auto& operator|(const std::shared_ptr<T>& lhs, const std::shared_ptr<U>& rhs) noexcept {
        if (lhs && rhs) *lhs | *rhs;
        return rhs;
    }

    // --- Components ---
    class file_src     : public observable {};
    class file_sink    : public observer { public: void on_notify() override {} };
    class json_encoder : public observer, public observable { public: void on_notify() override {} };
    class tcp_sender   : public observer { public: void on_notify() override {} };

    class HttpFlowContainer : public observer, public observable {
    public:
        using Processor = std::function<std::unique_ptr<data<http_msg>>(const std::unique_ptr<data<http_msg>>&, uint64_t)>;

        // Use string_view and move semantics for efficiency
        explicit HttpFlowContainer(std::string_view contentType, Processor proc = nullptr) 
            : content_type(contentType), processor(std::move(proc)) {}

        void on_notify() override { notify_observers(); }

    private:
        std::string content_type;
        Processor processor;
    };
}

int main() {
    using namespace flowpp;

    try {
        // Shared ownership setup
        auto src     = std::make_shared<file_src>();
        auto encoder = std::make_shared<json_encoder>();
        auto http    = std::make_shared<HttpFlowContainer>("application/json");
        auto sender  = std::make_shared<tcp_sender>();

        /**
         * FINAL SYNTAX:
         * We use the (void) cast to explicitly acknowledge the nodiscard return.
         * The pipeline is thread-safe, type-safe, and highly optimized.
         */
        (void)(src | encoder | http | sender);

        std::cout << "Pipeline Online: Optimized move semantics and noexcept piping." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
