#include <iostream>
#include <future> // Added for async execution
#include <flowpp/flowpp.hpp>

using namespace flowpp;

int main() {
    try {
        // --- Setup Graphs ---
        graph txGraph;
        graph rxGraph;

        // --- TX Logic (Outgoing) ---
        // We define the sender logic, which will push data to the receiver
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

        // --- Execution ---
        std::cout << "Launching Transmission Thread..." << std::endl;
        
        // Use std::async to run the TX graph in the background
        auto txFuture = std::async(std::launch::async, [&]() {
            return txGraph.run(); 
        });

        std::cout << "Starting Receiver Graph (Blocking)..." << std::endl;
        
        // This blocks the main thread while listening for incoming data
        auto rxResult = rxGraph.run(1000, INFINITE);
        
        // Clean up: Wait for the TX thread to wrap up if necessary
        txFuture.get();

        std::cout << "Receiver finished with result: " << rxResult << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
