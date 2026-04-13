#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>
#include <mutex>
#include <vector>

namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    const uint64_t INFINITE = 0xFFFFFFFFFFFFFFFF;

    // --- Base Interfaces ---
    class observer { 
    public: 
        virtual ~observer() = default; // Essential for safe inheritance
        virtual void on_notify() = 0; 
    };

    class observable {
    public:
        virtual ~observable() = default;
        
        void subscribe(observer* obs) {
            if (!obs) return;
            std::scoped_lock lock(mtx);
            observers.push_back(obs);
        }

        // Overload to support shared_ptr directly
        void subscribe(const std::shared_ptr<observer>& obs) {
            subscribe(obs.get());
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
    
    // 1. Pipe for raw references (Internal use)
    template <FlowObservable T, FlowObserver U>
    auto& operator|(T& lhs, U& rhs) {
        lhs.subscribe(&rhs);
        return rhs;
    }

    // 2. Pipe for shared_ptrs (External use)
    template <FlowObservable T, FlowObserver U>
    [[nodiscard]] auto& operator|(const std::shared_ptr<T>& lhs, const std::shared_ptr<U>& rhs) {
        if (lhs && rhs) *lhs | *rhs;
        return rhs;
    }

    // --- Components ---
    class file_src     : public observable {};
    class file_sink    : public observer { public: void on_notify() override {} };
    class json_encoder : public observer, public observable { public: void on_notify() override {} };
    class tcp_sender   : public observer { public: void on_notify() override {} };

    // Mock internal builder to show internal piping
    struct internal_builder : public observable {};

    class HttpFlowContainer : public observer, public observable {
    public:
        explicit HttpFlowContainer(std::string_view contentType) {
            // IMPROVEMENT: Use the pipe operator internally for consistency
            builder | *this; 
        }

        void on_notify() override { notify_observers(); }

    private:
        internal_builder builder;
    };
}

int main() {
    using namespace flowpp;

    try {
        // High-level assembly
        auto src     = std::make_shared<file_src>();
        auto encoder = std::make_shared<json_encoder>();
        auto http    = std::make_shared<HttpFlowContainer>("application/json");
        auto sender  = std::make_shared<tcp_sender>();

        // Fluent, thread-safe, and type-checked chain
        (void)(src | encoder | http | sender);

        std::cout << "Final Optimized Pipeline established." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
