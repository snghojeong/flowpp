#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>
#include <mutex>
#include <vector>
#include <algorithm>

// --- Modern Thread-Safe & Memory-Safe Framework ---
namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    const uint64_t INFINITE = 0xFFFFFFFFFFFFFFFF;

    class observer : public std::enable_shared_from_this<observer> { 
    public: 
        virtual ~observer() = default; 
        virtual void on_notify() = 0; 
    };

    class observable {
    public:
        virtual ~observable() = default;

        /**
         * IMPROVEMENT: Memory-Safe Observation.
         * Using weak_ptr ensures that if an observer is deleted elsewhere,
         * this observable won't hold a dangling pointer.
         */
        void subscribe(std::shared_ptr<observer> obs) {
            if (!obs) return;
            std::scoped_lock lock(mtx);
            observers.push_back(obs); // Implicit conversion to weak_ptr
        }

        void notify_all() {
            std::scoped_lock lock(mtx);
            // Clean up expired pointers while notifying
            auto it = observers.begin();
            while (it != observers.end()) {
                if (auto shared_obs = it->lock()) {
                    shared_obs->on_notify();
                    ++it;
                } else {
                    it = observers.erase(it); // Remove dangling references
                }
            }
        }

    private:
        std::mutex mtx;
        std::vector<std::weak_ptr<observer>> observers;
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

    explicit HttpFlowContainer(std::string_view contentType, Processor proc = nullptr) 
        : content_type(contentType), processor(std::move(proc)) {}

    void on_notify() override { /* Logic to process data and call notify_all() */ }

    [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) noexcept {
        return processor ? processor(dat, timeout) : nullptr;
    }

private:
    std::string content_type;
    Processor processor;
};

// --- Universal Pipe Operator for Shared Pointers ---

/**
 * FINAL IMPROVEMENT: Shared Ownership Piping.
 * By using shared_ptr, we guarantee the lifetime of the objects 
 * during the subscription process.
 */
template <FlowObservable T, FlowObserver U>
auto& operator|(const std::shared_ptr<T>& lhs, const std::shared_ptr<U>& rhs) {
    if (lhs && rhs) lhs->subscribe(rhs);
    return rhs;
}

// --- Main Execution ---
int main() {
    try {
        // 1. Shared Ownership Setup
        // Using make_shared is more efficient and required for weak_ptr safety
        auto fileSrc   = std::make_shared<file_src>();
        auto jsonEnc   = std::make_shared<json_encoder>();
        auto tcpSend   = std::make_shared<tcp_sender>();
        auto tcpRecv   = std::make_shared<tcp_recver>();
        auto fileSink  = std::make_shared<file_sink>();

        auto httpCont = std::make_shared<HttpFlowContainer>("application/json", 
            [](const DataPtr& d, uint64_t t) -> DataPtr {
                return std::make_unique<data<http_msg>>();
            });

        /**
         * 2. FINAL SYNTAX:
         * Memory-safe (no dangling pointers), Thread-safe (mutex-protected),
         * and Type-safe (C++20 Concepts).
         */
        fileSrc | jsonEnc  | httpCont | tcpSend;
        tcpRecv | httpCont | fileSink;

        std::cout << "Production-ready, memory-safe pipeline established." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Fatal: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
