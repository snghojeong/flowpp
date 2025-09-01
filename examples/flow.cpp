#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

using namespace flowpp;

using PortPtr = std::unique_ptr<ip_port>;
using GraphPtr = std::unique_ptr<graph>;
using TcpReceiverPtr = std::unique_ptr<tcp_receiver>;
using DataPtr = std::unique_ptr<data<json>>;

int main() {
    try {
        // Initialize transmission and reception graphs
        auto tcpTxGraph = std::make_unique<graph>();
        auto tcpRxGraph = std::make_unique<graph>();
        auto udpTxGraph = std::make_unique<graph>();
        auto udpRxGraph = std::make_unique<graph>();

        // Get components from the graphs
        auto fileSrc = tcpTxGraph->get<json_src>();
        auto fileWriter = tcpRxGraph->get<json_src>();
        auto tcpReceiver = tcpRxGraph->get<tcp_receiver>();
        auto tcpSender = tcpTxGraph->get<tcp_sender>();
        auto udpReceiver = udpRxGraph->get<udp_receiver>();
        auto udpSender = udpTxGraph->get<udp_sender>();

        // HTTP Configuration
        fileSrc["JSON"] | json_builder | http_builder | tcpSender["127.0.0.1"];
        tcpReceiver[port(80)] | http_parser[content_type("application/json")] | json_parser() | fileWriter("recv.json");
        tcpReceiver[port(8080)] | http_parser[content_type("plain/text")] | [](const std::string& txt) {
            std::cout << txt << std::endl;
        };

        // FTP Configuration
        tcpReceiver[port(22)] | ftp_parser | fileWriter("filename");

        // Run the receiver graph with a timeout of 1000ms and an infinite loop
        auto result = tcpRxGraph->run(1000 /* timeout */, INFINITE /* number of loop */);
        std::cout << "Result: " << result << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
