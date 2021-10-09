using content_type = flowpp::network::http::content_type;

class http_flow_container : public emitter, public receiver {
public:
  explicit flow_container() {
    json_encoder json_enc;
    this >> json_enc >> http_builder[content_type("application/json")] >> this;
  }

  virtual ~flow_container() { }
};

int main()
{
  http_flow_container http_container;

  src("file.json") >> http_container >> tcp_sender;
}
