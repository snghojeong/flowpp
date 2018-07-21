
class http_flow_container : public observer, public observable {
public:
  explicit flow_container() {
    this >> json_builder >> http_builder["application/jason"] >> this;
  }

  virtual ~flow_container() { }
};

int main()
{
  http_flow_container http_container;

  src >> http_container >> tcp_sender;
}
