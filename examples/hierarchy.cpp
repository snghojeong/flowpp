#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>
#include <mutex>
#include <vector>

namespace flowpp {
    // Basic types
    struct http_msg {};
    template<typename T> struct data {};
    const uint64_t INFINITE = 0xFFFFFFFFFFFFFFFF;

    // Interfaces
    class observer { 
    public: 
        virtual ~observer() = default; 
        virtual void on_notify() = 0; 
    };

    class observable {
    public:
        virtual ~observable() = default;
        
        void subscribe(std::shared_ptr<observer> obs) {
            if (!obs) return;
            std::scoped_lock lock(mtx);
            observers.push_back(obs);
        }

    protected:
        void notify_observers() {
            std::scoped_lock lock(mtx);
            for (auto it = observers.begin(); it != observers.end(); ) {
                if (auto shared_obs = it->lock()) {
                    shared_obs->on_notify();
                    ++it;
                } else {
                    it = observers.erase(it);
                }
            }
        }

    private:
        std::mutex mtx;
        std::vector<std::weak_ptr<observer>> observers;
    };

    // C++20 Concepts
    template<typename T>
    concept FlowObserver = std::derived_from<T, observer>;

    template<typename T>
    concept FlowObservable = std::derived_from<T, observable>;

    /**
     * IMPROVEMENT: Namespace-scoped, nodiscard pipe operator.
     * Returning the RHS by reference allows for fluid chaining.
     */
    template <FlowObservable T, FlowObserver U>
    [[nodiscard]] auto& operator|(const std::shared_ptr<T>& lhs, const std::shared_ptr<U>& rhs) {
        if (lhs && rhs) lhs->subscribe(rhs);
        return rhs;
    }

    // Concrete Components
    class file_src     : public observable {};
    class file_sink    : public observer { public: void on_notify() override {} };
    class json_encoder : public observer, public observable { public: void on_notify() override {} };
    class json_decoder : public observer, public observable { public: void on_notify() override {} };
    class tcp_sender   : public observer { public: void on_notify() override {} };
    class tcp_recver   : public observable {};

    class HttpFlowContainer : public observer, public observable {
    public:
        using Processor = std::function<std::unique_ptr<data<http_msg>>(const std::unique_ptr<data<http_msg>>&, uint64_t)>;

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
        // Initialize components as shared_ptrs for weak_ptr compatibility
        auto src     = std::make_shared<file_src>();
        auto encoder = std::make_shared<json_encoder>();
        auto http    = std::make_shared<HttpFlowContainer>("application/json");
        auto sender  = std::make_shared<tcp_sender>();

        /**
         * The pipe is now clean, thread-safe, and memory-safe.
         * We cast to void or just use the chain to satisfy [[nodiscard]].
         */
        (void)(src | encoder | http | sender);

        std::cout << "Optimized Pipeline established successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
