#include <cassert>
#include <string>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "api_request_client.hpp"
#include <doctest/doctest.h>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>
using namespace pjson;

class AddSceneTestFixture {
public:
  ExtendedDataStore dataStore;
  ExtendedControllerAPI api;
  api_request_client apiClient;
  const std::string PROPJECT_UUID = "1358840261077225472";

  AddSceneTestFixture() {

    // Login to get authentication token
    apiClient.login(
        "zebo.jiao@visla.us",
        "07e0a4368a53237a71d6f2b8040586ae6695df658d0469f94165d38fef3f5499");
    // apiClient.create_test_project();

    std::map<std::string, std::string> params = {
        {"projectUuid", PROPJECT_UUID}};
    nlohmann::json serverResponse =
        apiClient.get("/v3/project/" + PROPJECT_UUID + "/scene/list", params);

    // Initialize the data store with an empty project
    ExtendedProjectAndScenesVo pjson{serverResponse};
    dataStore.init(pjson);
    api.setDataStore(&dataStore);
  }

  ~AddSceneTestFixture() {
    // apiClient.remove_test_project();
  }
};

TEST_CASE_FIXTURE(AddSceneTestFixture, "/v3/project/{projectUuid}/scene/add") {
  // Test parameters based on API documentation
  int addPosition = 0;  // Insert at beginning
  int duration = 10000; // 10 seconds in milliseconds

  // Server API call using HTTP client
  std::map<std::string, std::string> params = {{"projectUuid", PROPJECT_UUID}};

  nlohmann::json requestBody;
  requestBody["addPosition"] = addPosition;
  requestBody["duration"] = duration;

  nlohmann::json serverResponse = apiClient.post(
      "/v3/project/" + PROPJECT_UUID + "/scene/add", requestBody, params);
      {
        ExtendedProjectSceneAddReqBody reqBody;
        reqBody.addPosition = addPosition;
        reqBody.duration = duration;
        auto r = api.addScene(reqBody);
        REQUIRE(r.success);
      }

  {
    // Validate server response structure according to API documentation
    REQUIRE(serverResponse.contains("code"));
    CHECK(serverResponse["code"] == 0);
    REQUIRE(serverResponse.contains("data"));

    auto serverData = serverResponse["data"];
    CHECK(serverData.contains("projectUuid"));
    CHECK(serverData.contains("scene"));
    CHECK(serverData.contains("sceneUuid"));

    auto serverScene = serverData["scene"];
    CHECK(serverScene.contains("sceneUuid"));
    CHECK(serverScene.contains("name"));
    CHECK(serverScene.contains("duration"));
    CHECK(serverScene.contains("sceneType"));
    CHECK(serverScene.contains("timeOffsetInProject"));

    // Validate server response values
    CHECK(serverScene["duration"].get<int>() == duration);
    CHECK(serverScene["timeOffsetInProject"].get<int>() ==
          0); // First scene should be at offset 0

    std::cout << "\n=== Server API Response ===" << std::endl;
    std::cout << "Scene UUID: " << serverScene["sceneUuid"] << std::endl;
    std::cout << "Scene Name: " << serverScene["name"] << std::endl;
    std::cout << "Scene Duration: " << serverScene["duration"] << std::endl;
    std::cout << "Scene Type: " << serverScene["sceneType"] << std::endl;
    std::cout << "Time Offset: " << serverScene["timeOffsetInProject"]
              << std::endl;
  }

  // Local ExtendedAPI comparison
}
