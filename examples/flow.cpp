#include <iostream>
#include <flowpp/flowpp.hpp>

using namespace flowpp;

int main() {
    // Set global logger for flowpp to see internal state transitions
    logging::set_level(logging::level::info);

    try {
        graph ioGraph;

        // --- Configuration Constants ---
        const std::string LOCAL_HOST = "127.0.0.1";
        const int HTTP_PORT = 80;
        const int TEXT_PORT = 8000;
        const int FTP_PORT  = 21;

        // --- TX Logic (Outgoing) ---
        // Improvement: Using 'optional' source to avoid crash if file is missing
        auto fileSrc   = ioGraph.get<json_src>("data.json", node_option::optional);
        auto tcpSender = ioGraph.get<tcp_sender>();

        fileSrc["mime/JSON"] | json_builder | http_builder | tcpSender[LOCAL_HOST];

        // --- RX Logic (Incoming) ---
        auto tcpReceiver = ioGraph.get<tcp_receiver>();
        
        // Improvement: Use a dedicated 'file_sink' with 'append' mode for logs
        auto jsonFileSink = ioGraph.get<json_sink>("recv.json", file_mode::overwrite);
        auto logFileSink  = ioGraph.get<file_sink>("system.log", file_mode::append);

        // JSON via HTTP (Port 80)
        tcpReceiver[port(HTTP_PORT)] 
            | http_parser[content_type("application/json")] 
            | json_parser() 
            | jsonFileSink;

        // Plain Text via HTTP (Port 8000)
        // Improvement: Directing logs to a file sink AND console (Tee-style)
        tcpReceiver[port(TEXT_PORT)] 
            | http_parser[content_type("text/plain")] 
            | [&](const std::string& txt) {
                std::cout << "[Incoming Log]: " << txt << std::endl;
                return txt; // Pass-through for the next node
              }
            | logFileSink;

        // FTP (Port 21)
        tcpReceiver[port(FTP_PORT)] 
            | ftp_parser 
            | ioGraph.get<file_sink>("ftp_recv.bin");

        // --- Execution ---
        std::cout << "Graph initialized. Starting event loop..." << std::endl;
        
        // Improvement: Use a non-infinite timeout for clean shutdowns
        // This allows the graph to process pending buffers before exiting.
        auto result = ioGraph.run(std::chrono::milliseconds(500), 5000);
        
        std::cout << "Execution finished. Result Code: " << result << std::endl;

    } catch (const std::exception& e) {
        // Catch-all for networking (EADDRINUSE) or IO (Permission Denied) errors
        std::cerr << "CRITICAL: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
