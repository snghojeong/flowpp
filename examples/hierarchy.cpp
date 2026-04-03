#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// Assuming these are defined in the flowpp namespace
namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    class observer { public: virtual ~observer() = default; };
    class observable {
    public:
        virtual ~observable() = default;
        void subscribe(observer* obs) { /* implementation */ }
    };
    // Mock classes for compilation context
    class file_src : public observable {};
    class file_sink : public observer {};
    class json_encoder : public observer, public observable {};
    class json_decoder : public observer, public observable {};
    class tcp_sender : public observer {};
    class tcp_recver : public observable {};
    namespace network { namespace http { class content_type {}; } }
}

using namespace flowpp;

using DataPtr = std::unique_ptr<data<http_msg>>;

class HttpFlowContainer : public observer, public observable {
public:
    explicit HttpFlowContainer(const std::string& contentType) {
        // Implementation logic
    }

    /**
     * IMPROVEMENT: Pass input by const reference.
     * This avoids unnecessary ownership transfer (moving) if you 
     * just need to process the data. Added [[nodiscard]].
     */
    [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) {
        // Simulate generating HTTP message data
        return std::make_unique<data<http_msg>>();
    }
};

/**
 * IMPROVED OPERATOR:
 * Single, clean overload for the base classes.
 */
observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

int main() {
    try {
        // Simulation of graph-based component retrieval (assuming graph objects exist)
        // We use reference_wrapper or raw pointers here as these are managed by the graph
        auto fileSrcPtr = std::make_unique<file_src>();
        auto fileSinkPtr = std::make_unique<file_sink>();
        auto jsonEncPtr = std::make_unique<json_encoder>();
        auto jsonDecPtr = std::make_unique<json_decoder>();
        auto httpContainerPtr = std::make_unique<HttpFlowContainer>("application/json");
        auto tcpRecverPtr = std::make_unique<tcp_recver>();
        auto tcpSenderPtr = std::make_unique<tcp_sender>();

        /**
         * The pipe operator now works seamlessly by dereferencing 
         * the unique_ptrs to their base class references.
         */
        *fileSrcPtr   | *jsonEncPtr       | *httpContainerPtr | *tcpSenderPtr;
        *tcpRecverPtr | *httpContainerPtr | *jsonDecPtr       | *fileSinkPtr;

        std::cout << "Flow graph established successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
