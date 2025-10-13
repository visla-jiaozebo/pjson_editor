#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif

#include <string>
#include <memory>
#include <pjson_editor/pjson_editor.hpp>
#include <pjson_editor/ExtendedAPI.h>

#ifdef EMSCRIPTEN
using namespace emscripten;
#endif
using namespace pjson;

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
std::string callEditor(const std::string& requestString) {
    if (!g_editor) {
        return R"({"error": "Editor not initialized"})";
    }
    
    try {
        nlohmann::json request_json = nlohmann::json::parse(requestString);
        
        // Create request
        auto req = std::make_shared<Request>();
        req->method = request_json.value("method", "GET");
        req->url = request_json.value("url", "");
        req->body = request_json.value("body", nlohmann::json{});
        
        if (request_json.contains("headers") && request_json["headers"].is_object()) {
            for (auto& [key, value] : request_json["headers"].items()) {
                if (value.is_string()) {
                    req->headers[key] = value.get<std::string>();
                }
            }
        }
        
        // Call the method
        auto resp = g_editor->call(req);
        
        // Convert response to JSON string
        nlohmann::json response_json;
        response_json["status_code"] = resp->status_code;
        response_json["headers"] = resp->headers;
        response_json["body"] = resp->body;
        
        return response_json.dump();
    } catch (const std::exception& e) {
        return "{\"error\": \"" + std::string(e.what()) + "\"}";
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
