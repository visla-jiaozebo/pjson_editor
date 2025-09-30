#include <cassert>
#include <memory>
#include <string>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "api_request_client.hpp"
#include <doctest/doctest.h>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>
#include <pjson_editor/pjson_editor.hpp>
using namespace pjson;

class TestSceneContext {
public:
  api_request_client apiClient;
  const std::string PROPJECT_UUID = "1358840261077225472";
  std::string sceneUuid;
  std::string localSceneUuid;
  std::shared_ptr<pjson::PJsonEditor> executor;
  TestSceneContext() {

    // Login to get authentication token
    apiClient.login(
        "zebo.jiao@visla.us",
        "07e0a4368a53237a71d6f2b8040586ae6695df658d0469f94165d38fef3f5499");
    // apiClient.create_test_project();
    nlohmann::json serverResponse =
        apiClient.get("/v3/project/" + PROPJECT_UUID + "/scene/list");

    // Initialize the data store with an empty project
    executor = std::make_shared<pjson::PJsonEditor>(serverResponse);
  }

  ~TestSceneContext() {
    if (sceneUuid.empty())
      return;
    {
      // api.removeScene(sceneUuid);
    }
  }
};

void check_scene_name(const nlohmann::json &resp, const std::string &name) {
  CHECK(resp["data"]["scene"]["name"] == name);
}

TEST_CASE_FIXTURE(TestSceneContext, "/v3/project/{projectUuid}/scene/") {
  { //"scene/add"
    nlohmann::json requestBody{{"addPosition", 1}, {"duration", 10000}};
    {
      nlohmann::json serverResponse = apiClient.post(
          "/v3/project/" + PROPJECT_UUID + "/scene/add", requestBody);
      CHECK(serverResponse["code"] == 0);
      sceneUuid = serverResponse["data"]["sceneUuid"].get<std::string>();
      check_scene_name(serverResponse, "New scene");
    }
    {
      auto r =
          executor->post({.method = "POST",
                          .url = "/v3/project/" + PROPJECT_UUID + "/scene/add",
                          .body = requestBody});
      check_scene_name(r.body, "New scene");
      localSceneUuid = r.body["data"]["sceneUuid"].get<std::string>();
    }
  }
  {
    // "scene/rename"
    nlohmann::json requestBody{{"sceneUuid", sceneUuid},
                               {"name", "Renamed Scene" + sceneUuid}};
    nlohmann::json serverResponse = apiClient.post(
        "/v3/project/" + PROPJECT_UUID + "/scene/rename", requestBody, "PUT");
    { check_scene_name(serverResponse, requestBody["name"]); }
    {
      requestBody["sceneUuid"] = localSceneUuid;
      auto r = executor->post(
          {.method = "PUT",
           .url = "/v3/project/" + PROPJECT_UUID + "/scene/rename",
           .body = requestBody});
      check_scene_name(r.body, requestBody["name"]);
    }
  }
  {
    apiClient.del("/v3/project/" + PROPJECT_UUID + "/scene/delete",
                  {{"sceneUuid", sceneUuid}});
    executor->post({.method = "DELETE",
                    .url = "/v3/project/" + PROPJECT_UUID + "/scene/delete",
                    .body = {{"sceneUuid", sceneUuid}}});
  }

  // Local ExtendedAPI comparison
}
