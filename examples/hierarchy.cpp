#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// Assuming these are defined in the flowpp namespace/library
using namespace flowpp;

using ContentTypePtr = std::unique_ptr<flowpp::network::http::content_type>;
using JsonEncoderPtr = std::unique_ptr<json_encoder>;
using FileSrcPtr = std::unique_ptr<file_src>;
using DataPtr = std::unique_ptr<data<http_msg>>;

class HttpFlowContainer : public observer, public observable {
public:
    explicit HttpFlowContainer(const std::string& contentType) {
        // IMPROVEMENT: Use the pipe operator consistently even in the constructor
        auto ct = std::make_unique<flowpp::network::http::content_type>(contentType);
        http_builder[std::move(ct)] | *this;
    }

    DataPtr poll(DataPtr dat, uint64_t timeout) {
        return std::make_unique<data<http_msg>>();
    }
};

using HttpContainerPtr = std::unique_ptr<HttpFlowContainer>;

/**
 * IMPROVED OPERATOR: 
 * By targeting the base classes 'observable' and 'observer', we don't 
 * need specific overloads for unique_ptr wrappers. The dereference 
 * operator (*) in main() handles the conversion naturally.
 */
observable& operator|(observable& lhs, observer& rhs) {
    lhs.subscribe(&rhs);
    return lhs;
}

int main() {
    try {
        // Simulation of graph-based component retrieval
        auto fileSrcPtr = tx_graph->get<FileSrc>();
        auto fileSinkPtr = rx_graph->get<file_sink>();
        auto jsonEncPtr = tx_graph->get<JsonEncoder>();
        auto httpContainerPtr = rx_graph->get<HttpFlowContainer>();
        auto tcpRecverPtr = rx_graph->get<tcp_recver>();
        auto tcpSenderPtr = tx_graph->get<tcp_sender>();
        
        // Ensure json_dec_uptr is defined or retrieved (was missing in original snippet)
        auto jsonDecPtr = rx_graph->get<json_decoder>();

        /**
         * CLEANER CHAINING:
         * Dereferencing unique_ptrs passes them as references to the 
         * base observer/observable types, making the logic type-safe 
         * and readable.
         */
        *fileSrcPtr   | *jsonEncPtr     | *httpContainerPtr | *tcpSenderPtr;
        *tcpRecverPtr | *httpContainerPtr | *jsonDecPtr      | *fileSinkPtr;

        tx_graph->run(INFINITE, INFINITE);
        rx_graph->run(INFINITE, INFINITE);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl; // Use cerr for errors
    }

    return 0;
}
