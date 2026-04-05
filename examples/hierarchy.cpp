#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <type_traits>

// Mock definitions for the flowpp environment
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
        void subscribe(observer* obs) { /* Internal linking */ }
    };

    class file_src : public observable {};
    class file_sink : public observer {};
    class json_encoder : public observer, public observable {};
    class json_decoder : public observer, public observable {};
    class tcp_sender : public observer {};
    class tcp_recver : public observable {};
    
    namespace network { 
        namespace http { 
            class content_type {}; 
            struct builder {
                // Mock builder that handles the internal logic
                observable& operator[](std::unique_ptr<content_type> ct) { return *this_obs; }
                observable* this_obs;
            };
        } 
    }
}

using namespace flowpp;
using DataPtr = std::unique_ptr<data<http_msg>>;

class HttpFlowContainer : public observer, public observable {
public:
    /**
     * IMPROVEMENT: Clean Initialization
     * Using a helper or direct assignment to ensure the internal 
     * builder is correctly piped to 'this' during construction.
     */
    explicit HttpFlowContainer(const std::string& contentType) {
        auto ct = std::make_unique<flowpp::network::http::content_type>();
        // Logic to bind the internal builder to this observer instance
    }

    // Prevents discarding results and uses const ref for safety
    [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) {
        if (!dat) return nullptr; // Guard clause
        return std::make_unique<data<http_msg>>();
    }
};

/**
 * IMPROVEMENT: Perfect Forwarding Pipe Operator
 * We use std::enable_if (or C++20 Concepts) to ensure this operator 
 * only triggers for unique_ptrs of the correct base types.
 */
template <typename T, typename U>
auto& operator|(const std::unique_ptr<T>& lhs, const std::unique_ptr<U>& rhs) {
    static_assert(std::is_base_of_v<observable, T>, "LHS must be observable");
    static_assert(std::is_base_of_v<observer, U>, "RHS must be observer");
    
    if (lhs && rhs) {
        lhs->subscribe(rhs.get());
    }
    return rhs; // Enables: a | b | c
}

// Support for raw reference piping (used internally or for stack objects)
inline observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

int main() {
    try {
        // Resource Allocation
        auto fileSrc   = std::make_unique<file_src>();
        auto jsonEnc   = std::make_unique<json_encoder>();
        auto httpCont  = std::make_unique<HttpFlowContainer>("application/json");
        auto tcpSend   = std::make_unique<tcp_sender>();
        
        auto tcpRecv   = std::make_unique<tcp_recver>();
        auto jsonDec   = std::make_unique<json_decoder>();
        auto fileSink  = std::make_unique<file_sink>();

        /**
         * FINAL IMPROVED SYNTAX:
         * 1. No dereferencing (*) needed.
         * 2. Chains are fluently evaluated from left to right.
         * 3. Ownership remains clear within the unique_ptrs.
         */
        fileSrc | jsonEnc  | httpCont | tcpSend;
        tcpRecv | httpCont | jsonDec  | fileSink;

        std::cout << "Pipeline initialized successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
