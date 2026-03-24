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
        std::cout << "DataHub: Monitoring ports 80, 21, and 8000..." << std::endl;
        // Run with a 100ms slice to keep the signal handler responsive
        while (keep_running) {
            ioGraph.run(std::chrono::milliseconds(100)); 
        }
        std::cout << "DataHub: Flushing buffers and exiting..." << std::endl;
    }

private:
    graph ioGraph;

    void setup_tx_logic() {
        auto fileSrc   = ioGraph.get<json_src>("data.json", node_option::optional);
        auto tcpSender = ioGraph.get<tcp_sender>();

        // TX: Added a small buffer (1MB) to decouple file reading from network speed
        fileSrc["mime/JSON"] 
            | buffer(1024 * 1024) 
            | json_builder 
            | http_builder 
            | tcpSender["127.0.0.1"];
    }

    void setup_rx_logic() {
        auto tcpReceiver = ioGraph.get<tcp_receiver>();
        auto jsonSink    = ioGraph.get<json_sink>("recv.json", file_mode::overwrite);
        auto logSink     = ioGraph.get<file_sink>("system.log", file_mode::append);

        // --- JSON Path (Port 80) ---
        // Added backpressure: If the JSON parser is slow, the buffer holds up to 500 items
        tcpReceiver[port(80)] 
            | http_parser[content_type("application/json")] 
            | buffer(500, strategy::backpressure) 
            | json_parser() 
            | jsonSink;

        // --- Log Path (Port 8000) ---
        // Added a "drop_oldest" strategy for logs to ensure the app never OOMs
        tcpReceiver[port(8000)] 
            | http_parser[content_type("text/plain")] 
            | buffer(100, strategy::drop_oldest) 
            | [](const std::string& msg) {
                std::cout << "[LOG]: " << msg << std::endl;
                return msg; 
              }
            | logSink;

        // --- FTP Path (Port 21) ---
        tcpReceiver[port(21)] 
            | ftp_parser 
            | buffer(2048 * 1024) // 2MB buffer for binary transfers
            | ioGraph.get<file_sink>("ftp_recv.bin");
    }
};

int main() {
    std::signal(SIGINT, signal_handler);

    try {
        // Optimization: Use multi-threaded execution for the graph's internal nodes
        // (Assuming flowpp is configured for 4 worker threads)
        DataHub hub;
        hub.start();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
