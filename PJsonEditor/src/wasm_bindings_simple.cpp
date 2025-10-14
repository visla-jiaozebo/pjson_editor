
#include "nlohmann/json_fwd.hpp"
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <memory>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/pjson_editor.hpp>
#include <string>

using namespace emscripten;
using namespace pjson;

//   std::string method;
//   std::string url;
//   nlohmann::json body;
//   std::map<std::string, std::string> headers;
//   std::string _requestId; // internal use only

std::shared_ptr<Request> makeGetRequest(const std::string &url) {
  auto req = std::make_shared<Request>();
  req->method = "GET";
  req->url = url;
  return req;
}

void smart_ptr_function(std::shared_ptr<Request> req) {
  // Just a placeholder function to demonstrate usage
  // In practice, you would call the PJsonEditor methods here
  emscripten::val console = emscripten::val::global("console");
  console.call<void>("log", std::string("Request Method: ") + req->method);
  console.call<void>("log", std::string("Request URL: ") + req->url);
  console.call<void>("log", std::string("Request Body: ") + req->body.dump());
  console.call<void>("log", std::string("Request Headers:"));
  for (const auto &[key, value] : req->headers) {
    console.call<void>("log", std::string("  ") + key + ": " + value);
  }
}

class PJsonEditorModule {
public:
  PJsonEditorModule() { editor = std::make_unique<PJsonEditor>(); }
  void update(const emscripten::val &scene_list_response) {
    if (scene_list_response["code"].as<int>() != 0) {
      console.call<void>(
          "warn",
          "Scene list response code is not 0: " +
              std::to_string(scene_list_response["code"].as<int>()) +
              ", message: " + scene_list_response["msg"].as<std::string>());
      return;
    }
    try {
      nlohmann::json scene_list_json = fromVal(scene_list_response["data"]);
      editor->update(scene_list_json);
    } catch (...) {
      console.call<void>("warn", "Exception updating with scene list");
      return;
    }
  }

  emscripten::val call(emscripten::val req) {
    emscripten::val respJS = emscripten::val::object();
    respJS.set("status_code", 404);
    try {
      auto request = makeRequest(req);
      auto resp = editor->call(request);
      respJS.set("status_code", resp->status_code);
      respJS.set("body",
                 JSON.call<emscripten::val>("parse", resp->body.dump()));
      respJS.set("_requestId", resp->_requestId);
    } catch (...) {
      console.call<void>("warn", std::string("Exception executing call"));
    }
    return respJS;
  }

  void feedServerResponse(emscripten::val respJS,
                          const emscripten::val &serverResp) {
    try {
      auto resp = makeResponse(respJS);
      nlohmann::json server_json = fromVal(serverResp);
      editor->feedServerResponse(resp, server_json);
    } catch (...) {
      console.call<void>("warn", "Exception checking server response");
      return;
    }
  }

  std::shared_ptr<Response> makeResponse(const emscripten::val respJS) {
    auto resp = std::make_shared<Response>();
    resp->status_code = respJS["status_code"].as<int>();
    if (respJS.hasOwnProperty("headers")) {
      emscripten::val headers = respJS["headers"];
      emscripten::val keys =
          emscripten::val::global("Object").call<emscripten::val>("keys",
                                                                  headers);
      int length = keys["length"].as<int>();
      for (int i = 0; i < length; i++) {
        std::string key = keys[i].as<std::string>();
        std::string value = headers[key].as<std::string>();
        resp->headers[key] = value;
      }
    }
    if (respJS.hasOwnProperty("_requestId")) {
      resp->_requestId = respJS["_requestId"].as<std::string>();
    }
    resp->body = fromVal(respJS["body"]);
    return resp;
  }

  std::shared_ptr<Request> makeRequest(const emscripten::val requestFromJS) {
    auto req = std::make_shared<Request>();
    // req->method = method;
    // req->url = url;
    // req->body = nlohmann::json::parse(body);
    if (requestFromJS.hasOwnProperty("body")) {
      req->body = fromVal(requestFromJS["body"]);
    }
    if (requestFromJS.hasOwnProperty("headers")) {
      emscripten::val headers = requestFromJS["headers"];
      emscripten::val keys =
          emscripten::val::global("Object").call<emscripten::val>("keys",
                                                                  headers);
      int length = keys["length"].as<int>();
      for (int i = 0; i < length; i++) {
        std::string key = keys[i].as<std::string>();
        std::string value = headers[key].as<std::string>();
        req->headers[key] = value;
      }
    }
    if (requestFromJS.hasOwnProperty("method")) {
      req->method = requestFromJS["method"].as<std::string>();
      req->url = requestFromJS["url"].as<std::string>();
    } else {
      req->method = "GET";
      req->url = requestFromJS.as<std::string>();
    }
    return req;
  }

private:
  std::unique_ptr<PJsonEditor> editor;
  emscripten::val console{emscripten::val::global("console")};
  emscripten::val JSON{emscripten::val::global("JSON")};

  inline nlohmann::json fromVal(const emscripten::val &v) {
    return nlohmann::json::parse(JSON.call<std::string>("stringify", v));
  }
};

// Emscripten bindings - Direct function exports without glue code
EMSCRIPTEN_BINDINGS(PJsonEditorModule) {
  class_<PJsonEditorModule>("PJsonEditorModule")
      .constructor<>()
      .function("update", &PJsonEditorModule::update)
      .function("call", &PJsonEditorModule::call)
      .function("makeRequest", &PJsonEditorModule::makeRequest)
      .function("makeResponse", &PJsonEditorModule::makeResponse)
      .function("feedServerResponse", &PJsonEditorModule::feedServerResponse);
  //   .function("dump_folder", &PJsonEditor::dump_folder);

  // function("createEditor", &createEditor);
  // function("updateEditor", &updateEditor);
  // function("callEditor", &callEditor);
  // function("feedServerResponse", &feedServerResponse);
  // function("setDumpFolder", &setDumpFolder);
};