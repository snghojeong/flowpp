#include <iostream>
#include <csignal>
#include <atomic>
#include <string>
#include <chrono>
#include <flowpp/flowpp.hpp>
#include <flowpp/zip.hpp> 
#include <flowpp/crypto.hpp> // Added for AES-256 support

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
    // Initialize with a 4-thread pool for heavy Crypto/Zip lifting
    DataHub() : ioGraph(execution_policy::parallel(4)), total_processed(0) {
        setup_tx_logic();
        setup_rx_logic();
        setup_health_monitor();
    }

    void start() {
        std::cout << "DataHub: Secure Vault mode active (AES-256 + GZIP)." << std::endl;
        while (keep_running) {
            ioGraph.run(std::chrono::milliseconds(100)); 
        }
    }

private:
    graph ioGraph;
    std::atomic<uint64_t> total_processed;

    // Secure key management (In production, load this from a Key Vault/Env)
    const std::string SECRET_KEY = "0123456789abcdef0123456789abcdef"; 

    void setup_health_monitor() {
        auto timer = ioGraph.get<timer_source>(std::chrono::seconds(5));
        timer | [&](const auto&) {
            std::cout << "[ENCRYPTED SINK STATUS] Verified Packets: " 
                      << total_processed.load() << std::endl;
        };
    }

    void setup_tx_logic() {
        auto fileSrc = ioGraph.get<json_src>("data.json", node_option::optional);
        auto tcpSender = ioGraph.get<tcp_sender>(
            retry_policy::exponential_backoff(std::chrono::seconds(2), std::chrono::seconds(30))
        );

        fileSrc["mime/JSON"] | buffer(1024) | json_builder | http_builder | tcpSender["127.0.0.1"];
    }

    void setup_rx_logic() {
        auto tcpReceiver = ioGraph.get<tcp_receiver>(node_option::auto_restart);
        
        // Output files are now .enc (Encrypted)
        auto secureSink = ioGraph.get<file_sink>("telemetry.dat.enc", file_mode::append);
        auto logSink    = ioGraph.get<file_sink>("system.log.enc", file_mode::append);

        // --- Secure JSON Path ---
        tcpReceiver[port(80)] 
            | http_parser[content_type("application/json")] 
            | json_parser() 
            | [&](const json& j) -> std::optional<json> {
                if (j.get<TelemetryData>().is_valid()) {
                    total_processed++;
                    return j;
                }
                return std::nullopt;
              }
            | zip_compressor(compression::gzip) // 1. Compress
            | aes_encryptor(SECRET_KEY)         // 2. Encrypt (AES-256-CBC)
            | secureSink;                       // 3. Save

        // --- Secure Log Path ---
        tcpReceiver[port(8000)] 
            | http_parser[content_type("text/plain")] 
            | zip_compressor(compression::gzip) 
            | aes_encryptor(SECRET_KEY) 
            | logSink;
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
