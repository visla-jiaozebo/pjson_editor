#include "nlohmann/json.hpp"
#include <map>
#include <memory>
#include <string>
#include <utility>
namespace pjson {

class ExtendedDataStore;
class ExtendedControllerAPI;
// define me a HttpRequest and HttpResponse structs
struct Request {
  std::string method;
  std::string url;
  nlohmann::json body;
  std::map<std::string, std::string> headers;
  std::string _requestId; // internal use only
};

struct Response {
  int status_code;
  std::map<std::string, std::string> headers;
  nlohmann::json body;
  std::string _requestId; // internal use only
};
using resp_ptr = std::shared_ptr<Response>;
using req_ptr = std::shared_ptr<Request>;

class PJsonEditor {
public:
  PJsonEditor();
  void update(const nlohmann::json &list_resp);
  resp_ptr call(req_ptr req);
  void feedServerResponse(resp_ptr, const nlohmann::json &);
  virtual ~PJsonEditor() = default;

  inline void dump_folder(const std::string &path){
    _dump_folder = path;
  };

private:
  std::unique_ptr<ExtendedControllerAPI> controller;
  std::shared_ptr<ExtendedDataStore> dataStore;
  std::string _dump_folder;
  uint32_t request_counter{0};
  std::map<std::string, std::pair<req_ptr, resp_ptr>> requests;
};

} // namespace pjson