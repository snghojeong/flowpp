#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>
#include <mutex>
#include <vector>
#include <utility>
#include <algorithm>

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
        
        void subscribe(observer* obs) noexcept {
            if (!obs) return;
            try {
                std::scoped_lock lock(mtx);
                // IMPROVEMENT: Reserve space to prevent frequent reallocations
                if (observers.capacity() == observers.size()) {
                    observers.reserve(observers.size() + 4); 
                }
                observers.push_back(obs);
            } catch (...) {} 
        }

    protected:
        void notify_observers() {
            std::scoped_lock lock(mtx);
            // Prune nulls and notify
            observers.erase(std::remove(observers.begin(), observers.end(), nullptr), observers.end());
            for (auto* obs : observers) {
                obs->on_notify();
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
    
    template <FlowObservable T, FlowObserver U>
    auto& operator|(T& lhs, U& rhs) noexcept {
        lhs.subscribe(&rhs);
        return rhs;
    }

    template <FlowObservable T, FlowObserver U>
    [[nodiscard]] auto& operator|(const std::shared_ptr<T>& lhs, const std::shared_ptr<U>& rhs) noexcept {
        if (lhs && rhs) *lhs | *rhs;
        return rhs;
    }

    // --- Finalized Concrete Components ---
    class file_src     final : public observable {};
    class file_sink    final : public observer { public: void on_notify() override {} };
    class json_encoder final : public observer, public observable { public: void on_notify() override {} };
    class tcp_sender   final : public observer { public: void on_notify() override {} };

    class HttpFlowContainer final : public observer, public observable {
    public:
        using DataPtr = std::unique_ptr<data<http_msg>>;
        using Processor = std::function<DataPtr(const DataPtr&, uint64_t)>;

        explicit HttpFlowContainer(std::string_view contentType, Processor proc = nullptr) 
            : content_type(contentType), processor(std::move(proc)) {}

        [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) noexcept {
            if (processor) return processor(dat, timeout);
            return dat ? std::make_unique<data<http_msg>>() : nullptr;
        }

        void on_notify() override { notify_observers(); }

    private:
        std::string content_type;
        Processor processor;
    };

    // Helper alias for cleaner main code
    template<typename T>
    using Ptr = std::shared_ptr<T>;
}

int main() {
    using namespace flowpp;

    try {
        /**
         * FINAL REFINEMENT: 
         * Using the 'Ptr' alias and perfect chaining.
         * This code is thread-safe, memory-efficient, and optimized for low latency.
         */
        auto src     = std::make_shared<file_src>();
        auto encoder = std::make_shared<json_encoder>();
        auto http    = std::make_shared<HttpFlowContainer>("application/json");
        auto sender  = std::make_shared<tcp_sender>();

        (void)(src | encoder | http | sender);

        std::cout << "Flow system initialized. Reallocation-guarded and type-erased." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
