#include <cassert>
#include <string>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "api_request_client.hpp"
#include <doctest/doctest.h>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>
using namespace pjson;

class TestSceneContext {
public:
  ExtendedDataStore dataStore;
  ExtendedControllerAPI api;
  api_request_client apiClient;
  const std::string PROPJECT_UUID = "1358840261077225472";
  std::string sceneUuid;

  TestSceneContext() {

    // Login to get authentication token
    apiClient.login(
        "zebo.jiao@visla.us",
        "07e0a4368a53237a71d6f2b8040586ae6695df658d0469f94165d38fef3f5499");
    // apiClient.create_test_project();
    nlohmann::json serverResponse =
        apiClient.get("/v3/project/" + PROPJECT_UUID + "/scene/list");

    // Initialize the data store with an empty project
    auto pjson = std::make_shared<ExtendedProjectAndScenesVo>(serverResponse);
    dataStore.init(pjson);
    api.setDataStore(&dataStore);
  }

  ~TestSceneContext() {
    if (sceneUuid.empty())
      return;
    {
      // api.removeScene(sceneUuid);
    }

  }
};

TEST_CASE_FIXTURE(TestSceneContext, "/v3/project/{projectUuid}/scene/") {
  { //"scene/add"
    nlohmann::json requestBody{{"addPosition", 1}, {"duration", 10000}};
    nlohmann::json serverResponse = apiClient.post(
        "/v3/project/" + PROPJECT_UUID + "/scene/add", requestBody);
    {
      CHECK(serverResponse["code"] == 0);
      sceneUuid = serverResponse["data"]["sceneUuid"].get<std::string>();
    }
    {
      ExtendedProjectSceneAddReqBody reqBody;
      reqBody.addPosition = requestBody["addPosition"];
      reqBody.duration = requestBody["duration"];
      auto r = api.addScene(reqBody);
      REQUIRE(r.success);
      
    }
  }
  {
    // "scene/rename"
    nlohmann::json requestBody{{"sceneUuid", sceneUuid}, {"name", "Renamed Scene" + sceneUuid}};
    nlohmann::json serverResponse = apiClient.post(
        "/v3/project/" + PROPJECT_UUID + "/scene/rename", requestBody,  "PUT");
    {
      CHECK(serverResponse["code"] == 0);
      // CHECK(serverResponse["data"]["name"] == requestBody["name"]);
    }
    {
      ExtendedProjectSceneRenameReqBody reqBody;
      reqBody.sceneUuid = requestBody["sceneUuid"];
      reqBody.name = requestBody["name"];
      auto r = api.renameScene(reqBody);
      REQUIRE(r.success);
      auto scene = dataStore.findScene(reqBody.sceneUuid);
      REQUIRE(scene != nullptr);
      CHECK(scene->name == requestBody["name"]);
    }
  }
  {
    apiClient.del("/v3/project/" + PROPJECT_UUID + "/scene/delete",
                  {{"sceneUuid", sceneUuid}});
    api.deleteScene({sceneUuid});
  }

  // Local ExtendedAPI comparison
}
