#include <iostream>
#include <csignal>
#include <atomic>
#include <string>
#include <flowpp/flowpp.hpp>

using namespace flowpp;

// Global flag for graceful shutdown
std::atomic<bool> keep_running{true};
void signal_handler(int) { keep_running = false; }

struct TelemetryData {
    int id;
    double value;
    std::string status;

    bool is_valid() const { return id > 0 && !status.empty(); }
};

class DataHub {
public:
    // Improvement: Configure the graph with a thread pool (e.g., 4 threads)
    DataHub() : ioGraph(execution_policy::parallel(4)) {
        setup_tx_logic();
        setup_rx_logic();
    }

    void start() {
        std::cout << "DataHub: Running in parallel mode (4 threads)." << std::endl;
        while (keep_running) {
            // Processing in small slices keeps the shutdown flag responsive
            ioGraph.run(std::chrono::milliseconds(100)); 
        }
    }

private:
    graph ioGraph;

    void setup_tx_logic() {
        auto fileSrc   = ioGraph.get<json_src>("data.json", node_option::optional);
        auto tcpSender = ioGraph.get<tcp_sender>();

        // TX path runs on its own thread in the pool
        fileSrc["mime/JSON"] 
            | buffer(1024 * 1024) 
            | json_builder 
            | http_builder 
            | tcpSender["127.0.0.1"];
    }

    void setup_rx_logic() {
        auto tcpReceiver = ioGraph.get<tcp_receiver>();
        auto validSink   = ioGraph.get<json_sink>("verified_data.json", file_mode::append);
        auto errorLog    = ioGraph.get<file_sink>("error.log", file_mode::append);

        // --- JSON Path (Port 80) ---
        // The parser and the sink can now run on different threads simultaneously
        tcpReceiver[port(80)] 
            | http_parser[content_type("application/json")] 
            | buffer(500, strategy::backpressure) 
            | json_parser() 
            | [&](const json& j) -> std::optional<json> {
                try {
                    if (j.get<TelemetryData>().is_valid()) return j;
                    throw std::runtime_error("Invalid Data");
                } catch (...) {
                    errorLog.push("Validation failed for: " + j.dump());
                    return std::nullopt;
                }
              }
            | validSink;

        // --- Log Path (Port 8000) ---
        tcpReceiver[port(8000)] 
            | http_parser[content_type("text/plain")] 
            | buffer(100, strategy::drop_oldest) 
            | [](const std::string& msg) {
                std::cout << "[LOG]: " << msg << std::endl;
                return msg; 
              }
            | ioGraph.get<file_sink>("system.log", file_mode::append);
    }
};

int main() {
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
