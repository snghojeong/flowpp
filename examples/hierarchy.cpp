
using content_type = flowpp::network::http::content_type;

class http_flow_container : public observer, public observable {
public:
  explicit flow_container() {
    this >> json_builder >> http_builder[content_type("application/json")] >> this;
  }

  virtual ~flow_container() { }
};

int main()
{
  http_flow_container http_container;

  src("JSON string") >> http_container >> tcp_sender;
}
