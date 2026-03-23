#include <iostream>
#include <csignal>
#include <atomic>
#include <flowpp/flowpp.hpp>

using namespace flowpp;

// Global flag for graceful shutdown
std::atomic<bool> keep_running{true};
void signal_handler(int) { keep_running = false; }

class DataHub {
public:
    DataHub() : ioGraph() {
        setup_tx_logic();
        setup_rx_logic();
    }

    void start() {
        std::cout << "DataHub: Starting event loop..." << std::endl;
        // Run until the atomic flag is flipped or internal timeout hit
        while (keep_running) {
            ioGraph.run(std::chrono::milliseconds(200), 1); 
        }
        std::cout << "DataHub: Shutting down safely..." << std::endl;
    }

private:
    graph ioGraph;

    void setup_tx_logic() {
        auto fileSrc   = ioGraph.get<json_src>("data.json", node_option::optional);
        auto tcpSender = ioGraph.get<tcp_sender>();

        // TX: JSON -> HTTP -> TCP
        fileSrc["mime/JSON"] | json_builder | http_builder | tcpSender["127.0.0.1"];
    }

    void setup_rx_logic() {
        auto tcpReceiver = ioGraph.get<tcp_receiver>();
        auto jsonSink    = ioGraph.get<json_sink>("recv.json", file_mode::overwrite);
        auto logSink     = ioGraph.get<file_sink>("system.log", file_mode::append);

        // Port 80: JSON Processor
        tcpReceiver[port(80)] 
            | http_parser[content_type("application/json")] 
            | json_parser() 
            | jsonSink;

        // Port 8000: Log Processor with a named lambda for clarity
        auto logger = [](const std::string& msg) {
            std::cout << "[LOG]: " << msg << std::endl;
            return msg; 
        };

        tcpReceiver[port(8000)] 
            | http_parser[content_type("text/plain")] 
            | logger 
            | logSink;
    }
};

int main() {
    // Register signal handler for Ctrl+C
    std::signal(SIGINT, signal_handler);

    try {
        DataHub hub;
        hub.start();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
