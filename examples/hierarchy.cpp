using namespace flowpp;

using content_type_uptr = unique_ptr<flowpp::network::http::content_type>;
using json_enc_uptr = unique_ptr<json_encoder>;
using file_src_uptr = unique_ptr<file_src>;
using data_uptr = unique_ptr<data<http_msg>>;

class http_flow_container : public observer, public observable {
public:
  explicit flow_container(std::string contents_type) {
    http_builder[make_unique<content_type_uptr>(contents_type)] | this;
  }

  data_uptr poll(uint64_t timeout) {
    return make_unique<data<http_msg>>();
  }
};

using http_container_uptr = unique_ptr<http_flow_container>;

int main()
{
  auto file_src_uptr = graph->get<file_src>();
  auto file_sink_uptr = graph->get<file_sink>();
  auto json_enc_uptr = graph->get<json_enc>();
  auto http_container_uptr = graph->get<http_flow_container>();
  auto tcp_recver_uptr = graph->get<tcp_recver>();
  auto tcp_sender_uptr = graph->get<tcp_sender>();

  file_src_uptr | json_enc_uptr | http_container_uptr | tcp_sender_uptr;
  tcp_recver_uptr | http_container_uptr | json_dec_uptr | file_sink_uptr;

  tx_graph->run(INFINITE, INFINITE);
  rx_graph->run(INFINITE, INFINITE);
}
