#include <cassert>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>
#include <pjson_editor/ApiMessage.h>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>
#include <pjson_editor/pjson_editor.hpp>
#include <regex>
#include <vector>

namespace pjson {

// Route handler function signature
using RouteHandler =
    std::function<Response(ExtendedControllerAPI *, const Request &)>;

// Route structure
struct Route {
  std::regex pattern;
  std::string method;
  RouteHandler handler;
};

// Forward declarations
Response createSuccessResponse(ApiResult &&result);
Response createErrorResponse(int statusCode, const std::string &message);

// Template function to create handlers for different request body types
template <typename ReqBodyType>
RouteHandler
createHandler(ApiResult (ExtendedControllerAPI::*method)(const ReqBodyType &)) {
  return [method](ExtendedControllerAPI *controller,
                  const Request &req) -> Response {
    try {
      ReqBodyType reqBody(req.body);
      ApiResult result = (controller->*method)(reqBody);
      return createSuccessResponse(std::move(result));
    } catch (const std::exception &e) {
      return createErrorResponse(400, "Invalid request body: " +
                                          std::string(e.what()));
    }
  };
}

// Static route table with member function pointers
static std::vector<Route> routes = {
    // POST routes using member function pointers
    {std::regex(R"(/v3/project/[^/]+/scene/add)"), "POST",
     createHandler(&ExtendedControllerAPI::addScene)},
    {std::regex(R"(/v3/project/[^/]+/scene/rename)"), "PUT",
     createHandler(&ExtendedControllerAPI::renameScene)},
    {std::regex(R"(/v3/project/[^/]+/scene/move)"), "POST",
     createHandler(&ExtendedControllerAPI::moveScene)},
    {std::regex(R"(/v3/project/[^/]+/scene/time/set)"), "POST",
     createHandler(&ExtendedControllerAPI::setSceneTime)},
    {std::regex(R"(/v3/project/[^/]+/scene/cut)"), "POST",
     createHandler(&ExtendedControllerAPI::cutScene)},
    {std::regex(R"(/v3/project/[^/]+/scene/split)"), "PUT",
     createHandler(&ExtendedControllerAPI::splitScene)},
    {std::regex(R"(/v3/project/[^/]+/scene/merge)"), "PUT",
     createHandler(&ExtendedControllerAPI::mergeScenes)},
    {std::regex(R"(/v3/project/[^/]+/scene/delete)"), "DELETE",
     createHandler(&ExtendedControllerAPI::deleteScene)},
    {std::regex(R"(/v3/project/[^/]+/scene/[^/]+/audio/add)"), "POST",
     createHandler(&ExtendedControllerAPI::addSceneAudio)},
    {std::regex(R"(/v3/project/[^/]+/scene/[^/]+/transition/set)"), "POST",
     createHandler(&ExtendedControllerAPI::setSceneTransition)},
    {std::regex(R"(/v3/project/[^/]+/scene/[^/]+/script/edit)"), "POST",
     createHandler(&ExtendedControllerAPI::editScript)},
};

PJsonEditor::PJsonEditor(const nlohmann::json &scene_list_resp) {
  // Initialize with config if needed
  auto pjson = std::make_shared<ExtendedProjectAndScenesVo>(scene_list_resp);
  dataStore = std::make_unique<ExtendedDataStore>();
  dataStore->init(pjson);
  controller = std::make_unique<ExtendedControllerAPI>();
  // Convert unique_ptr to shared_ptr for setDataStore
  std::shared_ptr<ExtendedDataStore> sharedDataStore(
      dataStore.get(), [](ExtendedDataStore *) {});
  controller->setDataStore(sharedDataStore);
}

// Route request using the route table
Response routeRequest(ExtendedControllerAPI *controller, const Request &req) {
  std::cout << req.method << " " << req.url << std::endl;

  // Find matching route
  for (const auto &route : routes) {
    if (route.method == req.method &&
        std::regex_match(req.url, route.pattern)) {
      return route.handler(controller, req);
    }
  }
  assert(false && "route not implement");
  // No matching route found
  return createErrorResponse(404, "Endpoint not found: " + req.method + " " +
                                      req.url);
}

Response PJsonEditor::call(Request req) {
  return routeRequest(controller.get(), req);
}

// Helper function to create success response
Response createSuccessResponse(ApiResult &&result) {
  Response resp;
  resp.status_code = 200;
  resp.headers["Content-Type"] = "application/json";

  if (result.isSuccess()) {
    resp.body["code"] = 0;
    resp.body["msg"] = "success";
    if (!result.data.empty()) {
      resp.body["data"] = std::move(result.data);
    }
  } else {
    resp.status_code = 400;
    resp.body["code"] = -1;
    resp.body["msg"] = ApiMessageHelper::getMessage(result.apiMessage);
  }
  return resp;
}

// Helper function to create error response
Response createErrorResponse(int statusCode, const std::string &message) {
  Response resp;
  resp.status_code = statusCode;
  resp.headers["Content-Type"] = "application/json";

  nlohmann::json responseJson;
  responseJson["code"] = -1;
  responseJson["msg"] = message;

  resp.body = responseJson.dump();
  return resp;
}

} // namespace pjson