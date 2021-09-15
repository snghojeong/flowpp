
using port = flowpp::network::port;
using content_type = flowpp::network::http::content_type;

int main()
{
  flowpp::graph tcp_graph();
  auto src = tcp_graph.get<str_src>();
  auto tcp_rx = tcp_graph.get<tcp_receiver>();
  auto tcp_tx = tcp_graph.get<tcp_sender>();

  // HTTP
  src["JSON"] >> json_builder >> http_builder >> tcp_tx; 
  tcp_rx[port(80)] >>   http_parser;
                        http_parser[content_type("application/json")] >> json_view;
                        http_parser[content_type("plain/text")] >> [] (const std::string& txt) { cout << txt; };

  // FTP
  tcp_rx[port(22)] >> ftp_parser >> file_writer("~/received_file");

  tcp_graph.wait(INFINITE /* timeout */, INFINITE /* number of loop */);

  return 0;
}
