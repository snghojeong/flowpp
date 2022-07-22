using content_type = flowpp::network::http::content_type;
using port_uptr = std::unique_ptr<flowpp::network::port>;
using graph_uptr = std::unique_ptr<graph>;
using tcp_receiver = flowpp::network::tcp_receiver;
using tcp_recv_uptr = std::unique_ptr<tcp_receiver>;

void main()
{
  graph_uptr tcp_graph;
  auto src = tcp_graph.get<str_src>();
  auto tcp_rx = tcp_graph.get<tcp_receiver>();
  auto tcp_tx = tcp_graph.get<tcp_sender>();

  // HTTP
  src["JSON"] | json_builder | http_builder | tcp_tx; 
  tcp_rx[port(80)] |    http_parser[content_type("application/json")] | file_writer("recv.json");
                        http_parser[content_type("plain/text")] | [] (const std::string& txt) { cout << txt; };

  // FTP
  tcp_rx[port(22)] | ftp_parser | file_writer("filename");

  tcp_graph.wait(-1 /* timeout */, -1 /* number of loop */);
}
