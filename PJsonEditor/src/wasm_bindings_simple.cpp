#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif

#include <string>
#include <memory>
#include "pjson_editor/ExtendedAPI.h"
#include "pjson_editor/ExtendedModels.h"

#ifdef EMSCRIPTEN
using namespace emscripten;
#endif
using namespace pjson;

/**
 * Simplified WebAssembly bindings for PJsonEditor
 * 
 * This provides basic functionality for demonstration purposes
 */

// Global API instance
static std::unique_ptr<ExtendedControllerAPI> g_api = nullptr;
static std::unique_ptr<ExtendedDataStore> g_dataStore = nullptr;

/**
 * Initialize the PJsonEditor with sample data
 */
bool initializeSampleProject() {
    try {
        g_dataStore = std::make_unique<ExtendedDataStore>();
        
        // Create a simple sample project
        ExtendedProject project;
        project.uuid = "sample-project-123";
        project.name = "Sample Project";
        
        // Add sample scenes
        ExtendedProjectScene scene1;
        scene1.uuid = "scene-1";
        scene1.name = "Scene 1";
        scene1.duration = 5000;
        scene1.timeOffsetInProject = 0;
        scene1.sceneType = SceneTypeEnum::DEFAULT;
        
        ExtendedProjectScene scene2;
        scene2.uuid = "scene-2";
        scene2.name = "Scene 2";
        scene2.duration = 3000;
        scene2.timeOffsetInProject = 5000;
        scene2.sceneType = SceneTypeEnum::DEFAULT;
        
        project.scenes = {scene1, scene2};
        g_dataStore->setProject(project);
        
        g_api = std::make_unique<ExtendedControllerAPI>(g_dataStore.get());
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * Get current project statistics
 */
std::string getStats() {
    if (!g_dataStore) {
        return R"({"error": "Not initialized"})";
    }
    
    const auto& project = g_dataStore->getProject();
    
    // Simple JSON building
    std::string result = "{";
    result += "\"projectName\": \"" + project.name + "\",";
    result += "\"projectUuid\": \"" + project.uuid + "\",";
    result += "\"totalScenes\": " + std::to_string(project.scenes.size());
    result += "}";
    
    return result;
}

/**
 * Rename a scene
 */
std::string renameSceneSimple(const std::string& sceneUuid, const std::string& newName) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    ExtendedProjectSceneRenameReqBody reqBody;
    reqBody.sceneUuid = sceneUuid;
    reqBody.name = newName;
    
    ApiResult result = g_api->renameScene(reqBody);
    
    std::string response = "{";
    response += "\"success\": " + std::string(result.success ? "true" : "false") + ",";
    response += "\"message\": \"" + result.message + "\"";
    response += "}";
    
    return response;
}

/**
 * Get list of scenes
 */
std::string getScenes() {
    if (!g_dataStore) {
        return R"({"error": "Not initialized"})";
    }
    
    const auto& project = g_dataStore->getProject();
    
    std::string result = "{\"scenes\": [";
    
    for (size_t i = 0; i < project.scenes.size(); ++i) {
        if (i > 0) result += ",";
        
        const auto& scene = project.scenes[i];
        result += "{";
        result += "\"uuid\": \"" + scene.uuid + "\",";
        result += "\"name\": \"" + scene.name + "\",";
        result += "\"duration\": " + std::to_string(scene.duration) + ",";
        result += "\"timeOffset\": " + std::to_string(scene.timeOffsetInProject);
        result += "}";
    }
    
    result += "]}";
    return result;
}

#ifdef EMSCRIPTEN
// Emscripten bindings
EMSCRIPTEN_BINDINGS(pjson_editor_simple) {
    function("initializeSampleProject", &initializeSampleProject);
    function("getStats", &getStats);
    function("renameSceneSimple", &renameSceneSimple);
    function("getScenes", &getScenes);
}
#endif
