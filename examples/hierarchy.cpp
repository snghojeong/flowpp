using content_type_uptr = unique_ptr<flowpp::network::http::content_type>;
using content_type_sptr = shared_ptr<flowpp::network::http::content_type>;
using json_enc_uptr = unique_ptr<json_encoder>;
using json_enc_sptr = shared_ptr<json_encoder>;
using http_container_uptr = unique_ptr<http_container>;
using file_src_uptr = unique_ptr<file_src>;

class http_flow_container : public emitter, public receiver {
public:
  explicit flow_container() {
    json_enc_uptr json_enc = make_unique<json_encoder>();
    this | json_enc | http_builder[unique_ptr<content_type>("application/json")] | this;
  }

  virtual ~flow_container() { }
};

int main()
{
  file_src_uptr file_src;
  http_flow_container_uptr http_container;

  file_src | http_container | tcp_sender;
  
  file_src->generate();
}
