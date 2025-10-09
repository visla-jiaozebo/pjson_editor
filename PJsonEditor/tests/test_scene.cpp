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
    // cleanup dump file
    std::string dump_file_path = "/tmp/test_scene_dump.txt";
    std::ofstream dump_file(dump_file_path, std::ios::trunc);
    dump_file.close();
    // enable dump
    executor->dump_file(dump_file_path);
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
    nlohmann::json serverResponse;
    {
      serverResponse = apiClient.post(
          "/v3/project/" + PROPJECT_UUID + "/scene/add", requestBody);
      CHECK(serverResponse["code"] == 0);
      sceneUuid = serverResponse["data"]["sceneUuid"].get<std::string>();
      check_scene_name(serverResponse, "New scene");
    }
    {
      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "POST",
                  .url = "/v3/project/" + PROPJECT_UUID + "/scene/add",
                  .body = requestBody}));
      CHECK(r->status_code == 404);
      executor->feedServerResponse(r, serverResponse);
      // list again to update local data store
      // executor->update(
      //     apiClient.get("/v3/project/" + PROPJECT_UUID + "/scene/list"));
    }
  }
  {
    // "scene/rename"
    nlohmann::json requestBody{{"sceneUuid", sceneUuid},
                               {"name", "Renamed Scene" + sceneUuid}};
    nlohmann::json serverResponse = apiClient.post(
        "/v3/project/" + PROPJECT_UUID + "/scene/rename", requestBody, "PUT");
    auto r = executor->call(std::make_shared<Request>(
        Request{.method = "PUT",
                .url = "/v3/project/" + PROPJECT_UUID + "/scene/rename",
                .body = requestBody}));
    auto &localResponse = r->body;
    CHECK(serverResponse["data"]["scene"]["name"] ==
          localResponse["data"]["scene"]["name"]);
    CHECK(serverResponse["data"]["scene"]["name"] == requestBody["name"]);
  }
  {
    // "scene/split"
    nlohmann::json requestBody{{"sceneUuid", sceneUuid},
                               {"splitTime", 5000}}; // split at 5 seconds
    std::string splitedSceneUuid[2];
    std::string localSplitedSceneUuid[2];
    {
      nlohmann::json serverResponse = apiClient.post(
          "/v3/project/" + PROPJECT_UUID + "/scene/split", requestBody, "PUT");

      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "PUT",
                  .url = "/v3/project/" + PROPJECT_UUID + "/scene/split",
                  .body = requestBody}));
      auto &localResponse = r->body;
      CHECK(serverResponse["code"] == localResponse["code"]);
      CHECK(serverResponse["data"].is_array() ==
            localResponse["data"].is_array());
      CHECK(serverResponse["data"].size() == localResponse["data"].size());
      splitedSceneUuid[0] =
          serverResponse["data"][0]["sceneUuid"].get<std::string>();
      splitedSceneUuid[1] =
          serverResponse["data"][1]["sceneUuid"].get<std::string>();

      localSplitedSceneUuid[0] =
          localResponse["data"][0]["sceneUuid"].get<std::string>();
      localSplitedSceneUuid[1] =
          localResponse["data"][1]["sceneUuid"].get<std::string>();
    }
    {
      // set-time
      nlohmann::json requestBody{
          {"sceneUuid", splitedSceneUuid[0]},
          {"duration",
           8000}}; // change the duration of the first scene to 8 seconds
      nlohmann::json serverResponse =
          apiClient.post("/v3/project/" + PROPJECT_UUID + "/scene/set-time",
                         requestBody, "PUT");
      requestBody["sceneUuid"] = localSplitedSceneUuid[0];
      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "PUT",
                  .url = "/v3/project/" + PROPJECT_UUID + "/scene/set-time",
                  .body = requestBody}));
      auto &localResponse = r->body;
      CHECK(serverResponse["code"] == localResponse["code"]);
    }
    {
      // scene/move
      nlohmann::json requestBody{
          {"sceneUuid", splitedSceneUuid[0]},
          {"afterSceneUuid",
           splitedSceneUuid[1]}}; // move the first scene after the second
      nlohmann::json serverResponse = apiClient.post(
          "/v3/project/" + PROPJECT_UUID + "/scene/move", requestBody, "PUT");
      requestBody["sceneUuid"] = localSplitedSceneUuid[0];
      requestBody["afterSceneUuid"] = localSplitedSceneUuid[1];
      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "PUT",
                  .url = "/v3/project/" + PROPJECT_UUID + "/scene/move",
                  .body = requestBody}));
      auto &localResponse = r->body;
      CHECK(serverResponse["code"] == localResponse["code"]);
    }
    {
      // scene/merge
      nlohmann::json requestBody{
          {"sceneUuids",
           {splitedSceneUuid[0],
            splitedSceneUuid[1]}} // merge the two scenes back
      };
      nlohmann::json serverResponse = apiClient.post(
          "/v3/project/" + PROPJECT_UUID + "/scene/merge", requestBody, "PUT");
      requestBody["sceneUuids"] = {localSplitedSceneUuid[0],
                                   localSplitedSceneUuid[1]};
      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "PUT",
                  .url = "/v3/project/" + PROPJECT_UUID + "/scene/merge",
                  .body = requestBody}));
      auto &localResponse = r->body;
      CHECK(serverResponse["code"] == localResponse["code"]);
      sceneUuid = serverResponse["data"]["sceneUuid"].get<std::string>();
    }
    {
      // add footage
      // {"sceneUuid":"1422511945931645372","bRoll":{"startTime":0,"endTime":10000,"entityType":"pexels","entityUuid":"7000000600003169","timeOffsetInProject":3600}}
      nlohmann::json requestBody{{"sceneUuid", sceneUuid},
                                 {"bRoll",
                                  {{"startTime", 0},
                                   {"endTime", 10000},
                                   {"entityType", "pexels"},
                                   {"entityUuid", "7000000600003169"},
                                   {"timeOffsetInProject", 3600}}}};
      nlohmann::json serverResponse =
          apiClient.post("/v3/project/" + PROPJECT_UUID + "/scene/add-footage",
                         requestBody, "PUT");
      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "PUT",
                  .url = "/v3/project/" + PROPJECT_UUID + "/scene/add-footage",
                  .body = requestBody}));
      auto &localResponse = r->body;
      CHECK(r->status_code == 404);
      executor->feedServerResponse(r, serverResponse);
      executor->update(apiClient.get("/v3/project/" + PROPJECT_UUID + "/scene/list"));
    }
    if (0) {
      // "scene/scale"
      {
        //   "scales": [
        //     {
        //       "scale": {
        //         "coordOffset": [
        //           0.1
        //         ],
        //         "value": 0.1
        //       },
        //       "timelineUuid": 996654321
        //     }
        //   ],
        //   "sceneUuid": "string"
        // }
        nlohmann::json requestBody{
            {"sceneUuid", sceneUuid},
            {"scales",
             {{{"scale", {{"coordOffset", {0.1}}, {"value", 0.8}}}},
              {"timelineUuid", 996654321}}}};

        nlohmann::json serverResponse =
            apiClient.post("/v3/project/" + PROPJECT_UUID + "/scene/scale",
                           requestBody, "PUT");
        auto r = executor->call(std::make_shared<Request>(
            Request{.method = "PUT",
                    .url = "/v3/project/" + PROPJECT_UUID + "/scene/scale",
                    .body = requestBody}));
        auto &localResponse = r->body;
        CHECK(serverResponse["code"] == localResponse["code"]);
      }
    }
  }
  {
    // "scene/delete"
    apiClient.del("/v3/project/" + PROPJECT_UUID + "/scene/delete",
                  {{"sceneUuid", sceneUuid}});
    executor->call(std::make_shared<Request>(
        Request{.method = "DELETE",
                .url = "/v3/project/" + PROPJECT_UUID + "/scene/delete",
                .body = {{"sceneUuid", sceneUuid}}}));
  }
  // Local ExtendedAPI comparison
}
