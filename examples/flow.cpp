#include <iostream>
#include <flowpp/flowpp.hpp>

using namespace flowpp;

int main() {
    try {
        // One graph to rule them all: Better resource scheduling
        graph ioGraph;

        // --- TX Logic (Outgoing) ---
        auto fileSrc   = ioGraph.get<json_src>("data.json");
        auto tcpSender = ioGraph.get<tcp_sender>();

        // Link TX: Source -> Builders -> Sender
        fileSrc | json_builder | http_builder | tcpSender["127.0.0.1"];

        // --- RX Logic (Incoming) ---
        auto tcpReceiver = ioGraph.get<tcp_receiver>();
        
        // Use a proper SINK for writing, not a SRC
        auto jsonFileSink = ioGraph.get<json_sink>("recv.json");
        auto genericSink  = ioGraph.get<file_sink>();

        // JSON via HTTP (Port 80)
        tcpReceiver[port(80)] 
            | http_parser[content_type("application/json")] 
            | json_parser() 
            | jsonFileSink;

        // Plain Text via HTTP (Port 8000)
        tcpReceiver[port(8000)] 
            | http_parser[content_type("text/plain")] 
            | [](const std::string& txt) { 
                std::cout << "Log: " << txt << std::endl; 
              };

        // FTP (Port 21 - Port 22 is usually SSH/SFTP)
        tcpReceiver[port(21)] | ftp_parser | genericSink("ftp_recv.bin");

        // --- Execution ---
        std::cout << "Starting Unified IO Graph..." << std::endl;
        
        // .run() now handles both TX and RX simultaneously 
        // because they belong to the same execution context.
        auto result = ioGraph.run();
        
        std::cout << "Graph execution halted. Status: " << result << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
