using namespace flowpp;

using port_uptr = std::unique_ptr<flowpp::network::port>;
using graph_uptr = std::unique_ptr<graph>;
using tcp_recv_uptr = std::unique_ptr<flowpp::network::tcp_receiver>;

void main()
{
  graph_uptr tcp_tx_graph;
  graph_uptr tcp_rx_graph;
  auto src = tcp_tx_graph.get<json_src>();
  auto tcp_rx = tcp_rx_graph.get<tcp_receiver>();
  auto tcp_tx = tcp_tx_graph.get<tcp_sender>();

  // HTTP
  src["JSON"] | json_builder | http_builder | tcp_tx; 
  tcp_rx[port(80)] |    http_parser[content_type("application/json")] | file_writer("recv.json");
  tcp_rx[port(8080)] |  http_parser[content_type("plain/text")] | [] (const std::string& txt) { cout << txt; };

  // FTP
  tcp_rx[port(22)] | ftp_parser | file_writer("filename");

  auto result = tcp_rx_graph.run(1000 /* timeout */, INFINITE /* number of loop */);
  std::cout << result << std::endl;
}
