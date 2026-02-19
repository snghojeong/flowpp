#include <iostream>
#include <flowpp/flowpp.hpp> // Assuming standard header naming

using namespace flowpp;

int main() {
    try {
        // Consolidate into functional groups (Transmission vs Reception)
        graph txGraph;
        graph rxGraph;

        // --- TX Logic (Outgoing) ---
        auto fileSrc   = txGraph.get<json_src>();
        auto tcpSender = txGraph.get<tcp_sender>();

        fileSrc["mime/JSON"] | json_builder | http_builder | tcpSender["127.0.0.1"];

        // --- RX Logic (Incoming) ---
        auto fileWriter  = rxGraph.get<json_src>();
        auto tcpReceiver = rxGraph.get<tcp_receiver>();
        
        // JSON via HTTP
        tcpReceiver[port(80)] 
            | http_parser[content_type("application/json")] 
            | json_parser() 
            | fileWriter("recv.json");

        // Plain Text via HTTP
        tcpReceiver[port(8000)] 
            | http_parser[content_type("plain/text")] 
            | [](const std::string& txt) { std::cout << "Log: " << txt << std::endl; };

        // FTP
        tcpReceiver[port(22)] | ftp_parser | fileWriter("filename");

        // Execute
        std::cout << "Starting receiver graph..." << std::endl;
        auto result = rxGraph.run(1000, INFINITE);
        
        std::cout << "Result: " << result << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
