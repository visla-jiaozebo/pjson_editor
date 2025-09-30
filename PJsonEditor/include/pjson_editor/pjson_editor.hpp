#include "nlohmann/json.hpp"
#include <map>
#include <string>
namespace pjson {

class ExtendedDataStore;
class ExtendedControllerAPI;
// define me a HttpRequest and HttpResponse structs
struct Request {
  std::string method;
  std::string url;
  nlohmann::json body;
  std::map<std::string, std::string> headers;
};

struct Response {
  int status_code;
  std::map<std::string, std::string> headers;
  nlohmann::json body;
};

class PJsonEditor {
public:
  PJsonEditor(const nlohmann::json &);
  Response get(Request req);
  Response post(Request req);
  virtual ~PJsonEditor() = default;

private:
  std::unique_ptr<ExtendedControllerAPI> controller;
  std::unique_ptr<ExtendedDataStore> dataStore;
};

} // namespace pjson