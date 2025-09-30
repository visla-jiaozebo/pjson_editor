#include <map>
#include <string>
namespace pjson {

class ExtendedDataStore;
class ExtendedControllerAPI;
// define me a HttpRequest and HttpResponse structs
struct Request {
  std::string method;
  std::string url;
  std::map<std::string, std::string> headers;
  std::string body;
};

struct Response {
  int status_code;
  std::map<std::string, std::string> headers;
  std::string body;
};

class PJsonEditor {
public:
  PJsonEditor(const std::string &);
  virtual Response get(Request) = 0;
  virtual Response post(Request) = 0;
  virtual ~PJsonEditor() = default;

private:
  std::unique_ptr<ExtendedControllerAPI> controller;
  std::unique_ptr<ExtendedDataStore> dataStore;
};

} // namespace pjson