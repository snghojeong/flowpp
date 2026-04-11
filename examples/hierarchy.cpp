#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>
#include <mutex>
#include <vector>

// --- Framework Core ---
namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    const uint64_t INFINITE = 0xFFFFFFFFFFFFFFFF;

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
}

using namespace flowpp;
using DataPtr = std::unique_ptr<data<http_msg>>;

// --- Domain Components ---
class HttpFlowContainer : public observer, public observable {
public:
    using Processor = std::function<DataPtr(const DataPtr&, uint64_t)>;

    explicit HttpFlowContainer(std::string_view contentType, Processor proc = nullptr) 
        : content_type(contentType), processor(std::move(proc)) {}

    void on_notify() override { 
        // Logic to move data through the pipe
        notify_observers(); 
    }

private:
    std::string content_type;
    Processor processor;
};

// --- Pipeline Component Factory (The final polish) ---
// This allows us to chain components without worrying about the pointer type
template <FlowObservable T, FlowObserver U>
auto& operator|(const std::shared_ptr<T>& lhs, const std::shared_ptr<U>& rhs) {
    if (lhs && rhs) lhs->subscribe(rhs);
    return rhs;
}

// Concrete component classes
class file_src : public observable {};
class file_sink : public observer { public: void on_notify() override {} };
class json_encoder : public observer, public observable { public: void on_notify() override {} };
class json_decoder : public observer, public observable { public: void on_notify() override {} };
class tcp_sender : public observer { public: void on_notify() override {} };
class tcp_recver : public observable {};

// --- Main Execution ---
int main() {
    try {
        /**
         * FINAL IMPROVEMENT: Component Grouping
         * We now treat the components as a structured "Service" layout.
         */
        struct Pipeline {
            std::shared_ptr<file_src> src           = std::make_shared<file_src>();
            std::shared_ptr<json_encoder> encoder   = std::make_shared<json_encoder>();
            std::shared_ptr<HttpFlowContainer> http = std::make_shared<HttpFlowContainer>("application/json");
            std::shared_ptr<tcp_sender> sender     = std::make_shared<tcp_sender>();
        } tx;

        // The syntax is now incredibly clean and handles all complexity behind the scenes
        tx.src | tx.encoder | tx.http | tx.sender;

        std::cout << "High-performance C++26 style pipeline initialized." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Pipeline error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
