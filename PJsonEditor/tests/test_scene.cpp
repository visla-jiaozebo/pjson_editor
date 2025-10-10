#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "api_request_client.hpp"
#include <doctest/doctest.h>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>
#include <pjson_editor/pjson_editor.hpp>
using namespace pjson;

TEST_CASE("Check CWD") {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != nullptr) {
    std::cout << "Current working directory: " << cwd << std::endl;
  }
}

class TestSceneContext {
public:
  std::shared_ptr<api_request_client> apiClient;
  const std::string PROPJECT_UUID = "1358840261077225472";
  std::string sceneUuid;
  std::shared_ptr<pjson::PJsonEditor> executor;
  TestSceneContext() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    apiClient = std::make_shared<api_request_client>(std::string(cwd) + "/remote");
    // Login to get authentication token
    apiClient->login(
        "zebo.jiao@visla.us",
        "07e0a4368a53237a71d6f2b8040586ae6695df658d0469f94165d38fef3f5499");
    // apiClient->create_test_project();
    nlohmann::json serverResponse =
        apiClient->get("/v3/project/" + PROPJECT_UUID + "/scene/list");

    // Initialize the data store with an empty project
    executor = std::make_shared<pjson::PJsonEditor>(serverResponse);
    executor->dump_folder(std::string(cwd) + "/local");
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
      serverResponse = apiClient->post(
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
      //     apiClient->get("/v3/project/" + PROPJECT_UUID + "/scene/list"));
    }
  }
  {
    // "scene/rename"
    nlohmann::json requestBody{{"sceneUuid", sceneUuid},
                               {"name", "Renamed Scene" + sceneUuid}};
    nlohmann::json serverResponse = apiClient->post(
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
      nlohmann::json serverResponse = apiClient->post(
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
          apiClient->post("/v3/project/" + PROPJECT_UUID + "/scene/set-time",
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
      nlohmann::json serverResponse = apiClient->post(
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
      nlohmann::json serverResponse = apiClient->post(
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
      std::string targetEntityUuid = "7000000600003169";
      // add footage
      // {"sceneUuid":"1422511945931645372","bRoll":{"startTime":0,"endTime":10000,"entityType":"pexels","entityUuid":"7000000600003169","timeOffsetInProject":3600}}
      nlohmann::json requestBody{{"sceneUuid", sceneUuid},
                                 {"bRoll",
                                  {{"startTime", 0},
                                   {"endTime", 10000},
                                   {"entityType", "pexels"},
                                   {"entityUuid", targetEntityUuid},
                                   {"timeOffsetInProject", 3600}}}};
      nlohmann::json serverResponse =
          apiClient->post("/v3/project/" + PROPJECT_UUID + "/scene/add-footage",
                          requestBody, "PUT");
      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "PUT",
                  .url = "/v3/project/" + PROPJECT_UUID + "/scene/add-footage",
                  .body = requestBody}));
      auto &localResponse = r->body;
      CHECK(r->status_code == 404);
      executor->feedServerResponse(r, serverResponse);
      auto listResp =
          apiClient->get("/v3/project/" + PROPJECT_UUID + "/scene/list");
      executor->update(listResp);
      // find the asset in the assets list which the sourceEntityUuid is
      // "7000000600003169", the whole json format is at
      // build/PJsonEditor/GET_v3_project_1358840261077225472_scene_list_response.json

      // Search for the asset with sourceEntityUuid "7000000600003169"
      std::string assetUuid;

      if (listResp.contains("data") && listResp["data"].contains("assets")) {
        for (const auto &asset : listResp["data"]["assets"]) {
          if (asset.contains("sourceEntityUuid") &&
              asset["sourceEntityUuid"].get<std::string>() ==
                  targetEntityUuid) {
            assetUuid = asset["assetUuid"].get<std::string>();
            break;
          }
        }
      }

      CHECK(assetUuid != "");
      // find the scene and assert the asset is in the scene's brolls
      auto sceneIt = std::find_if(listResp["data"]["scenes"].begin(),
                                  listResp["data"]["scenes"].end(),
                                  [&](const nlohmann::json &scene) {
                                    return scene["sceneUuid"] == sceneUuid;
                                  });
      CHECK(sceneIt != listResp["data"]["scenes"].end());
      auto theScene = *sceneIt;
      auto brollIt =
          std::find_if(theScene["brolls"].begin(), theScene["brolls"].end(),
                       [&](const nlohmann::json &broll) {
                         return broll["assetUuid"] == assetUuid;
                       });
      CHECK(brollIt != theScene["brolls"].end());
      auto timelineUuid = (*brollIt)["timelineUuid"].get<std::string>();
      CHECK(!timelineUuid.empty());
      // adjust footage
      {
        // {"sceneUuid":"1426163752314679401","startTime":1975,"endTime":10000,"timeOffsetInProject":33600,"timelineUuid":"1426163753342283978"}
        nlohmann::json requestBody{
            {"sceneUuid", sceneUuid},
            {"startTime", 1975},
            {"endTime", 10000},
            {"timeOffsetInProject", 33600},
            {"timelineUuid", timelineUuid}};

        nlohmann::json serverResponse = apiClient->post(
            "/v3/project/" + PROPJECT_UUID + "/scene/adjust-footage",
            requestBody, "PUT");
        auto r = executor->call(std::make_shared<Request>(
            Request{.method = "PUT",
                    .url = "/v3/project/" + PROPJECT_UUID +
                           "/scene/adjust-footage",
                    .body = requestBody}));
        auto &localResponse = r->body;
        CHECK(serverResponse["code"] == localResponse["code"]);
        CHECK(localResponse["data"]["scene"]["brolls"].is_array());
        // get the broll from data/scene/brolls/timelineUuid==timelineUuid
        auto brollIt = std::find_if(
            localResponse["data"]["scene"]["brolls"].begin(),
            localResponse["data"]["scene"]["brolls"].end(),
            [&](const nlohmann::json &broll) {
              return broll["timelineUuid"] == timelineUuid;
            });
        CHECK(brollIt != localResponse["data"]["scene"]["brolls"].end());
        CHECK((*brollIt)["startTime"] == 1975);
        CHECK((*brollIt)["timeOffsetInProject"] == 33600);
      }
      // "scene/scale"
      {
        nlohmann::json requestBody{
            {"sceneUuid", sceneUuid},
            {"scales",
             {{{"timelineUuid", timelineUuid},
               {"scale", {{"value", 0.71}, {"coordOffset", {0.25, 0.13}}}}}}}};

        nlohmann::json serverResponse =
            apiClient->post("/v3/project/" + PROPJECT_UUID + "/scene/scale",
                            requestBody, "PUT");
        auto r = executor->call(std::make_shared<Request>(
            Request{.method = "PUT",
                    .url = "/v3/project/" + PROPJECT_UUID + "/scene/scale",
                    .body = requestBody}));
        auto &localResponse = r->body;
        CHECK(serverResponse["code"] == localResponse["code"]);

        CHECK(localResponse.contains("data"));
        CHECK(localResponse["data"]["sceneUuid"] == sceneUuid);
        CHECK(localResponse["data"]["scales"].is_array());
        CHECK(localResponse["data"]["scales"].size() > 0);

        // Verify scale structure matches expected format
        auto &firstScale = localResponse["data"]["scales"][0];
        CHECK(firstScale["timelineUuid"] == timelineUuid);
        CHECK(firstScale.contains("scale"));
        CHECK(firstScale["scale"]["value"] == 0.71);

        CHECK(firstScale["scale"]["coordOffset"].is_array());
        CHECK(firstScale["scale"]["coordOffset"].size() == 2);
        CHECK(firstScale["scale"]["coordOffset"][0] == 0.25);
        CHECK(firstScale["scale"]["coordOffset"][1] == 0.13);
      }
    }
  }
  {
    // "scene/delete"
    apiClient->del("/v3/project/" + PROPJECT_UUID + "/scene/delete",
                   {{"sceneUuid", sceneUuid}});
    executor->call(std::make_shared<Request>(
        Request{.method = "DELETE",
                .url = "/v3/project/" + PROPJECT_UUID + "/scene/delete",
                .body = {{"sceneUuid", sceneUuid}}}));
  }
  // Local ExtendedAPI comparison
}

TEST_CASE_FIXTURE(TestSceneContext, "/v3/project/{projectUuid}/bgm") {
  { //"add-bgm"
    // {"entityType":"stock_audio","entityUuid":"1104599094787051520"}

    nlohmann::json requestBody{{"entityType", "stock_audio"}, {"entityUuid", "1104599094787051520"}};
    nlohmann::json serverResponse;
    {
      serverResponse = apiClient->post(
          "/v3/project/" + PROPJECT_UUID + "/add-bgm", requestBody);
      CHECK(serverResponse["code"] == 0);
      
      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "POST",
                  .url = "/v3/project/" + PROPJECT_UUID + "/add-bgm",
                  .body = requestBody}));
      CHECK(r->status_code == 404);
      executor->feedServerResponse(r, serverResponse);
      CHECK(serverResponse["data"]["bgms"].is_array());
      CHECK(serverResponse["data"]["bgms"].size() > 0);
      

    }
    {
      auto autoDucking = false;
      // edit-bgm
      // {"bgm":{"timelineUuid":"1426231537724035503","mixed":{"intro":true,"outro":true,"chorusLoop":false,"autoDucking":true}}}
      nlohmann::json requestBody{
          {"bgm",
           {{"timelineUuid", serverResponse["data"]["bgms"][0]["timelineUuid"]},
            {"mixed",
             {{"intro", true},
              {"outro", true},
              {"chorusLoop", false},
              {"autoDucking", autoDucking}}}}}};
      serverResponse = apiClient->post(
          "/v3/project/" + PROPJECT_UUID + "/edit-bgm", requestBody, "PUT");
      auto r = executor->call(std::make_shared<Request>(
          Request{.method = "PUT",
                  .url = "/v3/project/" + PROPJECT_UUID + "/edit-bgm",
                  .body = requestBody}));
      CHECK(r->body["code"] == serverResponse["code"]);
      CHECK(r->body["data"]["bgms"][0]["timelineUuid"] == serverResponse["data"]["bgms"][0]["timelineUuid"]);
      CHECK(autoDucking == serverResponse["data"]["bgms"][0]["mixed"]["autoDucking"]);
      CHECK(r->body["data"]["bgms"][0]["mixed"]["autoDucking"] == serverResponse["data"]["bgms"][0]["mixed"]["autoDucking"]);
    }
  }
}