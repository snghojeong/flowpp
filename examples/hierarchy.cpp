#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// Mock definitions to represent the flowpp environment
namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    
    class observer { 
    public: 
        virtual ~observer() = default; 
    };

    class observable {
    public:
        virtual ~observable() = default;
        void subscribe(observer* obs) { 
            // Internal logic to link components
        }
    };

    // Concrete component types
    class file_src : public observable {};
    class file_sink : public observer {};
    class json_encoder : public observer, public observable {};
    class json_decoder : public observer, public observable {};
    class tcp_sender : public observer {};
    class tcp_recver : public observable {};
    
    namespace network { 
        namespace http { 
            class content_type {}; 
        } 
    }
}

using namespace flowpp;
using DataPtr = std::unique_ptr<data<http_msg>>;

class HttpFlowContainer : public observer, public observable {
public:
    explicit HttpFlowContainer(const std::string& contentType) {
        // Initialization logic
    }

    // Using [[nodiscard]] and const ref for better resource management
    [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) {
        return std::make_unique<data<http_msg>>();
    }
};

/**
 * IMPROVEMENT 1: Base Operator
 * Standard piping for raw references.
 */
observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

/**
 * IMPROVEMENT 2: Smart Pointer Overload
 * This template allows us to pipe unique_ptrs directly. 
 * It automatically handles the dereferencing logic.
 */
template <typename T, typename U>
auto& operator|(const std::unique_ptr<T>& lhs, const std::unique_ptr<U>& rhs) {
    static_assert(std::is_base_of<observable, T>::value, "LHS must be observable");
    static_assert(std::is_base_of<observer, U>::value, "RHS must be observer");
    *lhs | *rhs;
    return rhs; // Return RHS to allow further chaining
}

int main() {
    try {
        // Components managed by unique_ptrs (simulating retrieval from a graph)
        auto fileSrc   = std::make_unique<file_src>();
        auto fileSink  = std::make_unique<file_sink>();
        auto jsonEnc   = std::make_unique<json_encoder>();
        auto jsonDec   = std::make_unique<json_decoder>();
        auto httpCont  = std::make_unique<HttpFlowContainer>("application/json");
        auto tcpRecv   = std::make_unique<tcp_recver>();
        auto tcpSend   = std::make_unique<tcp_sender>();

        /**
         * CLEANER SYNTAX:
         * We no longer need to prefix everything with '*'
         * The template operator handles the unique_ptr wrappers for us.
         */
        fileSrc | jsonEnc | httpCont | tcpSend;
        tcpRecv | httpCont | jsonDec | fileSink;

        std::cout << "Data pipeline established successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
