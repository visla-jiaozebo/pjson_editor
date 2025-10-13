#include <cassert>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <pjson_editor/ApiMessage.h>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>
#include <pjson_editor/pjson_editor.hpp>
#include <random>
#include <regex>
#include <sstream>
#include <utility>
#include <vector>

namespace pjson {
namespace fs = std::filesystem;
static fs::path create_temp_file(const std::string &prefix = "tmp") {
  fs::path temp_dir = fs::temp_directory_path();
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 999999999);

  fs::path temp_file;
  do {
    std::ostringstream oss;
    oss << prefix << "_" << dis(gen);
    temp_file = temp_dir / oss.str();
  } while (fs::exists(temp_file)); // ensure it doesn’t exist yet

  // create the file
  std::ofstream(temp_file).close();
  return temp_file;
}
// Route handler function signature
using RouteHandler =
    std::function<resp_ptr(ExtendedControllerAPI *, const Request &)>;
using FeedServerRespHandler =
    std::function<void(ExtendedControllerAPI *, const nlohmann::json &)>;

// Route structure
struct Route {
  std::regex pattern;
  std::string method;
  RouteHandler handler;
  FeedServerRespHandler feedHandler{nullptr};
};

// Forward declarations
static resp_ptr createSuccessResponse(ApiResult &&result);
static resp_ptr createErrorResponse(int statusCode, const std::string &message);

// Template function to create handlers for different request body types
template <typename ReqBodyType>
RouteHandler
createHandler(ApiResult (ExtendedControllerAPI::*method)(const ReqBodyType &)) {
  return [method](ExtendedControllerAPI *controller,
                  const Request &req) -> resp_ptr {
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

    Route{std::regex(R"(/v3/project/[^/]+/scene/add)"), "POST",
          // createHandler(&ExtendedControllerAPI::addScene),
          nullptr, &ExtendedControllerAPI::addSceneFeedServerResp},
    {std::regex(R"(/v3/project/[^/]+/scene/rename)"), "PUT",
     createHandler(&ExtendedControllerAPI::renameScene)},
    {std::regex(R"(/v3/project/[^/]+/scene/move)"), "PUT",
     createHandler(&ExtendedControllerAPI::moveScene)},
    {std::regex(R"(/v3/project/[^/]+/scene/set-time)"), "PUT",
     createHandler(&ExtendedControllerAPI::setSceneTime)},

    {std::regex(R"(/v3/project/[^/]+/scene/add-footage)"), "PUT", nullptr},
    {std::regex(R"(/v3/project/[^/]+/scene/adjust-footage)"), "PUT",
      createHandler(&ExtendedControllerAPI::adjustFootage)},
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
    //  scene/scale from docs/API-snapshot.md
    {std::regex(R"(/v3/project/[^/]+/scene/scale)"), "PUT",
     createHandler(&ExtendedControllerAPI::setSceneScale)},
    {std::regex(R"(/v3/project/[^/]+/add-bgm)"), "POST", nullptr,
     &ExtendedControllerAPI::updateSceneList},
    {std::regex(R"(/v3/project/[^/]+/edit-bgm)"), "PUT",
     createHandler(&ExtendedControllerAPI::editBgm), &ExtendedControllerAPI::updateSceneList},
};

PJsonEditor::PJsonEditor():controller(std::make_unique<ExtendedControllerAPI>()),
                           dataStore(std::make_shared<ExtendedDataStore>()) {
  controller->setDataStore(dataStore);
}

void PJsonEditor::update(const nlohmann::json &list_resp) {
  // Initialize with config if needed
  auto pjson = std::make_shared<ExtendedProjectAndScenesVo>(list_resp);
  dataStore->init(pjson);
}

// Route request using the route table
static resp_ptr routeRequest(ExtendedControllerAPI *controller,
                             const Request &req) {
  std::cout << req.method << " " << req.url << std::endl;

  // Find matching route
  for (const auto &route : routes) {
    if (route.method == req.method &&
        std::regex_match(req.url, route.pattern)) {
      if (!route.handler) {
        break;
      }
      return route.handler(controller, req);
    }
  }
  return createErrorResponse(404, "Endpoint not found: " + req.method + " " +
                                      req.url);
}
static std::string extract_url_path(const std::string &url) {
  // Find the position after the protocol (http:// or https://)
  size_t protocol_pos = url.find("://");
  if (protocol_pos == std::string::npos) {
    return url; // No protocol found, assume it's already a path
  }

  // Find the first '/' after the protocol and domain
  size_t path_start = url.find('/', protocol_pos + 3);
  if (path_start == std::string::npos) {
    return "/"; // No path found, return root
  }

  // Extract everything after the domain
  std::string path = url.substr(path_start);

  // Remove query parameters if present
  size_t query_pos = path.find('?');
  if (query_pos != std::string::npos) {
    path = path.substr(0, query_pos);
  }

  // Remove fragment if present
  size_t fragment_pos = path.find('#');
  if (fragment_pos != std::string::npos) {
    path = path.substr(0, fragment_pos);
  }

  return path;
}

resp_ptr PJsonEditor::call(req_ptr req) {
#ifndef NDEBUG
  if (!_dump_folder.empty()) {
    fs::create_directory(_dump_folder);
    // assert(ok && "Failed to create dump folder");
    auto url_path = extract_url_path(req->url);
    std::replace(url_path.begin(), url_path.end(), '/', '_');
    fs::path dump_file_path =
        _dump_folder + "/" + req->method + url_path + "_req.json";
    std::ofstream dump_file(dump_file_path.c_str());
    dump_file << req->body.dump(2) << "\n";
    dump_file.close();
  }
#endif
  assert(req->_requestId.empty() && "Request ID should be empty");
  req->_requestId = std::to_string(++request_counter);
  auto r = routeRequest(controller.get(), *req);
  r->_requestId = req->_requestId;
  requests[req->_requestId] = std::make_pair(req, r);
#ifndef NDEBUG
  if (!_dump_folder.empty() && r->status_code != 404) {
    fs::create_directory(_dump_folder);
    // assert(ok && "Failed to create dump folder");
    auto url_path = extract_url_path(req->url);
    std::replace(url_path.begin(), url_path.end(), '/', '_');
    fs::path dump_file_path =
        _dump_folder + "/" + req->method + url_path + "_rsp.json";
    std::ofstream dump_file(dump_file_path.c_str());
    dump_file << r->body.dump(2) << "\n";
  }
#endif
  return r;
}

void PJsonEditor::feedServerResponse(resp_ptr r, const nlohmann::json &resp) {
  assert(!r->_requestId.empty() && "Request ID should not be empty");
  assert(requests.find(r->_requestId) != requests.end() &&
         "Request ID not found");
  auto req = requests[r->_requestId].first;
  for (const auto &route : routes) {
    if (route.method == req->method &&
        std::regex_match(req->url, route.pattern)) {
      if (route.feedHandler) {
        route.feedHandler(controller.get(), resp);
      }
      break;
    }
  }
}

// Helper function to create success response
static resp_ptr createSuccessResponse(ApiResult &&result) {
  resp_ptr resp = std::make_shared<Response>();
  resp->status_code = 200;
  resp->headers["Content-Type"] = "application/json";

  if (result.isSuccess()) {
    resp->body["code"] = 0;
    resp->body["msg"] = "success";
    if (!result.data.empty()) {
      resp->body["data"] = std::move(result.data);
    }
  } else {
    resp->status_code = 400;
    resp->body["code"] = -1;
    resp->body["msg"] = ApiMessageHelper::getMessage(result.apiMessage);
  }
  return resp;
}

// Helper function to create error response
static resp_ptr createErrorResponse(int statusCode,
                                    const std::string &message) {
  resp_ptr resp = std::make_shared<Response>();
  resp->status_code = statusCode;
  resp->headers["Content-Type"] = "application/json";

  nlohmann::json &responseJson = resp->body;
  responseJson["code"] = -1;
  responseJson["msg"] = message;
  return resp;
}

} // namespace pjson