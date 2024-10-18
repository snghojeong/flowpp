#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

using namespace flowpp;

using ContentTypePtr = std::unique_ptr<flowpp::network::http::content_type>;
using JsonEncoderPtr = std::unique_ptr<json_encoder>;
using FileSrcPtr = std::unique_ptr<file_src>;
using DataPtr = std::unique_ptr<data<http_msg>>;

// Class representing the HTTP flow container
class HttpFlowContainer : public observer, public observable {
public:
    // Constructor initializing the container with content type
    explicit HttpFlowContainer(const std::string& contentType) {
        // Using unique_ptr to manage content type resource
        http_builder[std::make_unique<flowpp::network::http::content_type>(contentType)] | this;
    }

    // Poll function returning data pointer
    DataPtr poll(uint64_t timeout) {
        // Simulate generating HTTP message data
        return std::make_unique<data<http_msg>>();
    }
};

using HttpContainerPtr = std::unique_ptr<HttpFlowContainer>;

// Overload the pipe operator for chaining observables with observers
observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

// Overload the pipe operator for chaining with unique_ptrs
observable& operator|(observable& lhs, ObserverPtr& rhs) {
    lhs.subscribe(rhs.get());
    return lhs;
}

int main() {
    try {
        // Get components from tx and rx graphs using make_unique
        auto fileSrcPtr = tx_graph->get<FileSrc>();
        auto fileSinkPtr = rx_graph->get<file_sink>();
        auto jsonEncPtr = tx_graph->get<JsonEncoder>();
        auto httpContainerPtr = rx_graph->get<HttpFlowContainer>();
        auto tcpRecverPtr = rx_graph->get<tcp_recver>();
        auto tcpSenderPtr = graph->get<tcp_sender>();

        // Chain components using the overloaded pipe operator
        *fileSrcPtr | *jsonEncPtr | *httpContainerPtr | *tcpSenderPtr;
        *tcpRecverPtr | *httpContainerPtr | *json_dec_uptr | *fileSinkPtr;

        // Run tx and rx graphs with INFINITE loops
        tx_graph->run(INFINITE, INFINITE);
        rx_graph->run(INFINITE, INFINITE);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
