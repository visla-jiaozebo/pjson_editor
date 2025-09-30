#include <pjson_editor/pjson_editor.hpp>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>

namespace pjson {

PJsonEditor::PJsonEditor(const std::string &scene_list_resp) {
  // Initialize with config if needed
      ExtendedProjectAndScenesVo pjson{scene_list_resp};
    dataStore.init(pjson);
    api.setDataStore(&dataStore);
}
Response PJsonEditor::get(Request req) {
  // Implement GET request handling
  Response resp;
  resp.status_code = 501; // Not Implemented
  resp.body = "GET method not implemented";
  return resp;
}

Response PJsonEditor::post(Request req) {
  // Implement POST request handling
  Response resp;
  resp.status_code = 501; // Not Implemented
  resp.body = "POST method not implemented";
  return resp;
}

} // namespace pjson