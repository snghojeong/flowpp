using content_type = flowpp::network::http::content_type;
using port_uptr = std::unique_ptr<flowpp::network::port>;
using graph_uptr = std::unique_ptr<graph>;

int main(int, char**)
{
  graph_uptr tcp_graph;
  auto src = tcp_graph.get<str_src>();
  auto tcp_rx = tcp_graph.get<tcp_receiver>();
  auto tcp_tx = tcp_graph.get<tcp_sender>();

  // HTTP
  src["JSON"] | json_builder | http_builder | tcp_tx; 
  tcp_rx[port(80)] |    http_parser;
                        http_parser[content_type("application/json")] | json_view;
                        http_parser[content_type("plain/text")] | [] (const std::string& txt) { cout << txt; };

  // FTP
  tcp_rx[port(22)] | ftp_parser | file_writer("filename");

  tcp_graph.wait(10000 /* timeout */, INFINITE /* number of loop */);

  return 0;
}
