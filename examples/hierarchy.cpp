#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <vector>

// --- Mock Framework Definitions ---
namespace flowpp {
    struct http_msg {};
    template<typename T> struct data {};
    
    class observer { public: virtual ~observer() = default; };
    class observable {
    public:
        virtual ~observable() = default;
        void subscribe(observer* obs) { /* Internal link logic */ }
    };

    class file_src : public observable {};
    class file_sink : public observer {};
    class json_encoder : public observer, public observable {};
    class json_decoder : public observer, public observable {};
    class tcp_sender : public observer {};
    class tcp_recver : public observable {};
}

using namespace flowpp;
using DataPtr = std::unique_ptr<data<http_msg>>;

// --- Component Definition ---
class HttpFlowContainer : public observer, public observable {
public:
    explicit HttpFlowContainer(const std::string& contentType) {
        // Initialization using provided content type
    }

    [[nodiscard]] DataPtr poll(const DataPtr& dat, uint64_t timeout) {
        return dat ? std::make_unique<data<http_msg>>() : nullptr;
    }
};

// --- Improved Pipe Operators ---

// 1. Raw Reference Pipe (The Primitive)
inline observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

// 2. Smart Pointer Pipe (The Convenience)
// Uses SFINAE to ensure we only pipe flowpp components
template <typename T, typename U, 
          typename = std::enable_if_t<std::is_base_of_v<observable, T> && 
                                     std::is_base_of_v<observer, U>>>
auto& operator|(const std::unique_ptr<T>& lhs, const std::unique_ptr<U>& rhs) {
    if (lhs && rhs) *lhs | *rhs;
    return rhs;
}

// --- Pipeline Encapsulation ---
struct NetworkPipeline {
    std::unique_ptr<file_src> src;
    std::unique_ptr<json_encoder> encoder;
    std::unique_ptr<HttpFlowContainer> http;
    std::unique_ptr<tcp_sender> sender;

    static NetworkPipeline Create(const std::string& config) {
        NetworkPipeline p;
        p.src = std::make_unique<file_src>();
        p.encoder = std::make_unique<json_encoder>();
        p.http = std::make_unique<HttpFlowContainer>(config);
        p.sender = std::make_unique<tcp_sender>();

        // Internal wiring happens once here
        p.src | p.encoder | p.http | p.sender;
        
        return p;
    }
};

// --- Main Execution ---
int main() {
    try {
        // IMPROVEMENT: Main is now clean and high-level
        auto tx_pipeline = NetworkPipeline::Create("application/json");
        
        // Simulating the RX side similarly
        auto tcpRecv  = std::make_unique<tcp_recver>();
        auto jsonDec  = std::make_unique<json_decoder>();
        auto fileSink = std::make_unique<file_sink>();

        // Fluid chaining for custom/one-off graphs
        tcpRecv | jsonDec | fileSink;

        std::cout << "Systems Online. Pipeline encapsulated and running." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Critical Failure: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
