
#include "nlohmann/json_fwd.hpp"
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <memory>
#include <pjson_editor/pjson_editor.hpp>
#include <pjson_editor/ExtendedAPI.h>

using namespace emscripten;
using namespace pjson;


//   std::string method;
//   std::string url;
//   nlohmann::json body;
//   std::map<std::string, std::string> headers;
//   std::string _requestId; // internal use only

std::shared_ptr<Request> makeGetRequest(const std::string& url) {
    auto req = std::make_shared<Request>();
    req->method = "GET";
    req->url = url;
    return req;
}

std::shared_ptr<Request> makeRequest(const emscripten::val requestFromJS) {
    auto req = std::make_shared<Request>();
    // req->method = method;
    // req->url = url;
    // req->body = nlohmann::json::parse(body);
    if (requestFromJS.hasOwnProperty("body")) {
        req->body = nlohmann::json::parse(requestFromJS["body"].as<std::string>());
    } 
    if (requestFromJS.hasOwnProperty("headers")) {
        emscripten::val headers = requestFromJS["headers"];
        emscripten::val keys = emscripten::val::global("Object").call<emscripten::val>("keys", headers);
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
    } 
    else {
        req->method = "GET";
        req->url = requestFromJS.as<std::string>();
    }
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
    for (const auto& [key, value] : req->headers) {
        console.call<void>("log", std::string("  ") + key + ": " + value);
    }
}

EMSCRIPTEN_BINDINGS(parameters) {
  class_<Request>("Request")
        .smart_ptr_constructor("Request", &makeRequest);
  function("dumpRequest", &smart_ptr_function);
};
/**
 * Simple WebAssembly bindings for PJsonEditor
 * Exports PJsonEditor public methods without glue code
 */

// Global PJsonEditor instance
static std::unique_ptr<PJsonEditor> g_editor = nullptr;

/**
 * Create a new PJsonEditor instance
 */
bool createEditor() {
    try {
        g_editor = std::make_unique<PJsonEditor>();
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * Update the editor with JSON data
 */
bool updateEditor(const std::string& jsonString) {
    if (!g_editor) {
        return false;
    }
    
    try {
        nlohmann::json json_data = nlohmann::json::parse(jsonString);
        g_editor->update(json_data);
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * Call the editor with a request and return response as JSON string
 */
emscripten::val callEditor(std::shared_ptr<Request> req) {
    if (!g_editor) {
        return emscripten::val(R"({"error": "Editor not initialized"})");
    }
    
    try {
         // Call the method
        auto resp = g_editor->call(req);
        emscripten::val response_json = emscripten::val::object();
        response_json.set("status_code", resp->status_code);
        response_json.set("body", emscripten::val(resp->body.dump()));
        return response_json;
    } catch (const std::exception& e) {
        return emscripten::val("error: " + std::string(e.what()));
    }
}

/**
 * Feed server response to the editor
 */
bool feedServerResponse(const std::string& responseString, const std::string& serverJsonString) {
    if (!g_editor) {
        return false;
    }
    
    try {
        nlohmann::json response_json = nlohmann::json::parse(responseString);
        nlohmann::json server_json = nlohmann::json::parse(serverJsonString);
        
        // Create response object
        auto resp = std::make_shared<Response>();
        resp->status_code = response_json.value("status_code", 200);
        resp->body = response_json.value("body", nlohmann::json{});
        
        if (response_json.contains("headers") && response_json["headers"].is_object()) {
            for (auto& [key, value] : response_json["headers"].items()) {
                if (value.is_string()) {
                    resp->headers[key] = value.get<std::string>();
                }
            }
        }
        
        g_editor->feedServerResponse(resp, server_json);
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * Set dump folder path
 */
void setDumpFolder(const std::string& path) {
    if (g_editor) {
        g_editor->dump_folder(path);
    }
}

#ifdef EMSCRIPTEN
// Emscripten bindings - Direct function exports without glue code
EMSCRIPTEN_BINDINGS(PJsonEditorModule) {
    function("createEditor", &createEditor);
    function("updateEditor", &updateEditor);
    function("callEditor", &callEditor);
    function("feedServerResponse", &feedServerResponse);
    function("setDumpFolder", &setDumpFolder);
}
#endif
