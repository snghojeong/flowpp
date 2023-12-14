using namespace flowpp;

using content_type_uptr = unique_ptr<flowpp::network::http::content_type>;
using json_enc_uptr = unique_ptr<json_encoder>;
using file_src_uptr = unique_ptr<file_src>;
using data_uptr = unique_ptr<data>;

class http_flow_container : public observer, public observable {
public:
  explicit flow_container(std::string contents_type) {
    http_builder[make_unique<content_type_uptr>(contents_type)] | this;
  }
};

int main()
{
  auto file_src = graph->get<file_src_uptr>();
  auto json_enc = graph->get<json_enc_uptr>();
  http_flow_container_uptr http_container;

  file_src | json_enc | http_container | tcp_sender;
  tcp_recver | http_container | json_dec | file_sink;

  graph->run(INFINITE, INFINITE);
}
