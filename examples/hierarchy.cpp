#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <stdexcept>
#include <type_traits>

// --- Mock Framework Definitions ---
namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    const uint64_t INFINITE = 0xFFFFFFFFFFFFFFFF;

    class observer { public: virtual ~observer() = default; };
    class observable {
    public:
        virtual ~observable() = default;
        void subscribe(observer* obs) { if(obs) /* Internal link */ ; }
    };

    class file_src : public observable {};
    class file_sink : public observer {};
    class json_encoder : public observer, public observable {};
    class json_decoder : public observer, public observable {};
    class tcp_sender : public observer {};
    class tcp_recver : public observable {};
    
    // Mock Graph to simulate your tx_graph/rx_graph
    struct Graph {
        template<typename T>
        T* get() { static T instance; return &instance; } 
        void run(uint64_t, uint64_t) {}
    };
}

using namespace flowpp;
using DataPtr = std::unique_ptr<data<http_msg>>;

// --- Component Definition ---
class HttpFlowContainer : public observer, public observable {
public:
    // IMPROVEMENT: Use string_view to avoid copying the string buffer
    explicit HttpFlowContainer(std::string_view contentType) {
        // Initialization logic using contentType
    }

    // IMPROVEMENT: Mark as 'noexcept' if it doesn't throw, and use [[nodiscard]]
    [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) noexcept {
        try {
            return dat ? std::make_unique<data<http_msg>>() : nullptr;
        } catch (...) {
            return nullptr;
        }
    }
};

// --- Improved Pipe Operators ---

/**
 * IMPROVEMENT: Using a template that handles both Raw Pointers and Smart Pointers.
 * This is the most flexible approach for a library-style pipe operator.
 */
template <typename T, typename U>
auto& operator|(T* lhs, U* rhs) {
    static_assert(std::is_base_of_v<observable, T>, "LHS must be observable");
    static_assert(std::is_base_of_v<observer, U>, "RHS must be observer");
    if (lhs && rhs) lhs->subscribe(rhs);
    return rhs;
}

// Support for chaining when mixed with raw references
inline observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

// --- Main Execution ---
int main() {
    // Simulated global graphs
    auto tx_graph = std::make_unique<Graph>();
    auto rx_graph = std::make_unique<Graph>();

    try {
        /**
         * IMPROVEMENT: Raw Pointer Usage.
         * Since 'tx_graph' owns the components, we get raw pointers.
         * This avoids 'double-free' errors and represents "view" access.
         */
        auto* fileSrc    = tx_graph->get<file_src>();
        auto* jsonEnc    = tx_graph->get<json_encoder>();
        auto* httpCont   = rx_graph->get<HttpFlowContainer>(); // Shared in your logic
        auto* tcpSend    = tx_graph->get<tcp_sender>();
        
        auto* tcpRecv    = rx_graph->get<tcp_recver>();
        auto* jsonDec    = rx_graph->get<json_decoder>();
        auto* fileSink   = rx_graph->get<file_sink>();

        /**
         * The pipe operator now works on raw pointers returned by the graph.
         * This is much safer than wrapping them in unique_ptr at the call site.
         */
        fileSrc | jsonEnc | httpCont | tcpSend;
        tcpRecv | httpCont | jsonDec | fileSink;

        tx_graph->run(INFINITE, INFINITE);
        rx_graph->run(INFINITE, INFINITE);

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
