using content_type_uptr = unique_ptr<flowpp::network::http::content_type>;
using json_enc_uptr = unique_ptr<json_encoder>;
using file_src_uptr = unique_ptr<file_src>;

class http_flow_container : public emitter, public receiver {
public:
  explicit flow_container() {
    this | http_builder[make_unique<content_type_uptr>("application/json")] | this;
  }

  virtual ~flow_container() { }
};

int main()
{
  auto file_src = graph->get<file_src_uptr>();
  auto json_enc = graph->get<json_enc_uptr>();
  http_flow_container_uptr http_container;

  file_src | json_enc | http_container | tcp_sender;

  graph->run();
}
