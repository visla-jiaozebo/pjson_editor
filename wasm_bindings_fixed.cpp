#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <exception>
#include "pjson_editor/ExtendedAPI.h"
#include "pjson_editor/ExtendedModels.h"

#ifdef EMSCRIPTEN
using namespace emscripten;
#endif
using namespace pjson;

/**
 * WebAssembly bindings for PJsonEditor
 * 
 * This file provides JavaScript-accessible bindings for the core PJsonEditor functionality
 * allowing web applications to manipulate project scenes and timelines.
 */

// Global API instance
static std::unique_ptr<ExtendedControllerAPI> g_api = nullptr;
static std::unique_ptr<ExtendedDataStore> g_dataStore = nullptr;

/**
 * Initialize the PJsonEditor API with project data
 * @param projectJson - JSON string containing the project data
 * @return bool - true if initialization successful
 */
bool initializeAPI(const std::string& projectJson) {
    try {
        g_dataStore = std::make_unique<ExtendedDataStore>();
        
        // Initialize with a basic project structure
        ExtendedProjectAndScenesVo project;
        project.projectUuid = "default-project";
        project.ownerUuid = "default-owner";
        project.project.uuid = "default-project";
        project.project.name = "Default Project";
        project.project.ownerUuid = "default-owner";
        
        g_dataStore->init(project);
        
        g_api = std::make_unique<ExtendedControllerAPI>();
        g_api->setDataStore(g_dataStore.get());
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

/**
 * Get the current project data as JSON string
 * @return string - JSON representation of the project
 */
std::string getProjectJson() {
    if (!g_dataStore) {
        return R"({"error": "API not initialized"})";
    }
    
    // For now, return a simple representation
    const auto& project = g_dataStore->getProject();
    nlohmann::json result;
    result["projectUuid"] = project.projectUuid;
    result["ownerUuid"] = project.ownerUuid;
    result["sceneCount"] = project.scenes.size();
    return result.dump();
}

/**
 * Helper function to convert ApiResult to JSON string
 */
std::string apiResultToJson(const ApiResult& result) {
    nlohmann::json j;
    j["success"] = result.success;
    j["message"] = result.message;
    j["patch"] = result.patch;
    return j.dump();
}

/**
 * Add a new scene to the project
 * @param name - Scene name
 * @param duration - Scene duration in milliseconds (optional)
 * @return string - JSON result of the operation
 */
std::string addScene(const std::string& name, int duration = 5000) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    try {
        ExtendedProjectSceneAddReqBody reqBody;
        reqBody.name = name;
        reqBody.duration = duration;
        
        ApiResult result = g_api->addScene(reqBody);
        return apiResultToJson(result);
    } catch (const std::exception& e) {
        return R"({"success": false, "message": "Exception: )" + std::string(e.what()) + R"("})";
    }
}

/**
 * Rename an existing scene
 * @param sceneUuid - UUID of the scene to rename
 * @param newName - New name for the scene
 * @return string - JSON result of the operation
 */
std::string renameScene(const std::string& sceneUuid, const std::string& newName) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    try {
        ExtendedProjectSceneRenameReqBody reqBody;
        reqBody.uuid = sceneUuid;
        reqBody.newName = newName;
        
        ApiResult result = g_api->renameScene(reqBody);
        return apiResultToJson(result);
    } catch (const std::exception& e) {
        return R"({"success": false, "message": "Exception: )" + std::string(e.what()) + R"("})";
    }
}

/**
 * Delete a scene from the project
 * @param sceneUuid - UUID of the scene to delete
 * @return string - JSON result of the operation
 */
std::string deleteScene(const std::string& sceneUuid) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    try {
        ExtendedProjectSceneDeleteReqBody reqBody;
        reqBody.uuid = sceneUuid;
        
        ApiResult result = g_api->deleteScene(reqBody);
        return apiResultToJson(result);
    } catch (const std::exception& e) {
        return R"({"success": false, "message": "Exception: )" + std::string(e.what()) + R"("})";
    }
}

/**
 * Split a scene into two scenes
 * @param sceneUuid - UUID of the scene to split
 * @param splitTimeMs - Time position in milliseconds where to split
 * @return string - JSON result of the operation
 */
std::string splitScene(const std::string& sceneUuid, int splitTimeMs) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    try {
        ExtendedProjectSceneSplitReqBody reqBody;
        reqBody.sceneUuid = sceneUuid;
        reqBody.splitTime = splitTimeMs;
        
        ApiResult result = g_api->splitScene(reqBody);
        return apiResultToJson(result);
    } catch (const std::exception& e) {
        return R"({"success": false, "message": "Exception: )" + std::string(e.what()) + R"("})";
    }
}

/**
 * Merge multiple scenes into one
 * @param sceneUuidsJson - JSON array of scene UUIDs to merge
 * @return string - JSON result of the operation
 */
std::string mergeScenes(const std::string& sceneUuidsJson) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    try {
        nlohmann::json j = nlohmann::json::parse(sceneUuidsJson);
        std::vector<std::string> sceneUuids = j.get<std::vector<std::string>>();
        
        ExtendedProjectSceneMergeReqBody reqBody;
        reqBody.sceneUuids = sceneUuids;
        
        ApiResult result = g_api->mergeScenes(reqBody);
        return apiResultToJson(result);
    } catch (const std::exception& e) {
        return R"({"success": false, "message": "Exception: )" + std::string(e.what()) + R"("})";
    }
}

/**
 * Add footage to a scene
 * @param sceneUuid - UUID of the target scene
 * @param assetUuid - UUID of the asset to add
 * @param timeOffsetMs - Time offset in the scene (milliseconds)
 * @param durationMs - Duration of the footage (milliseconds)
 * @return string - JSON result of the operation
 */
std::string addFootage(const std::string& sceneUuid, const std::string& assetUuid, 
                      int timeOffsetMs, int durationMs) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    try {
        ProjectSceneFootageAddReqBody reqBody;
        reqBody.sceneUuid = sceneUuid;
        reqBody.assetUuid = assetUuid;
        reqBody.timeOffsetInScene = timeOffsetMs;
        reqBody.duration = durationMs;
        
        ApiResult result = g_api->addFootage(reqBody);
        return apiResultToJson(result);
    } catch (const std::exception& e) {
        return R"({"success": false, "message": "Exception: )" + std::string(e.what()) + R"("})";
    }
}

/**
 * Get project statistics
 * @return string - JSON with project statistics
 */
std::string getProjectStats() {
    if (!g_dataStore) {
        return R"({"error": "API not initialized"})";
    }
    
    try {
        const auto& project = g_dataStore->getProject();
        
        nlohmann::json stats;
        stats["projectUuid"] = project.projectUuid;
        stats["ownerUuid"] = project.ownerUuid;
        stats["sceneCount"] = project.scenes.size();
        stats["timelineCount"] = project.timelines.size();
        stats["assetCount"] = project.assets.size();
        
        // Calculate total duration
        int totalDuration = 0;
        for (const auto& scene : project.scenes) {
            totalDuration += scene.duration;
        }
        stats["totalDurationMs"] = totalDuration;
        
        return stats.dump();
    } catch (const std::exception& e) {
        return R"({"error": "Exception: )" + std::string(e.what()) + R"("})";
    }
}

/**
 * Get a list of all scenes in the project
 * @return string - JSON array of scene information
 */
std::string getSceneList() {
    if (!g_dataStore) {
        return R"({"error": "API not initialized"})";
    }
    
    try {
        const auto& project = g_dataStore->getProject();
        nlohmann::json scenes = nlohmann::json::array();
        
        for (const auto& scene : project.scenes) {
            nlohmann::json sceneInfo;
            sceneInfo["uuid"] = scene.uuid;
            sceneInfo["name"] = scene.name;
            sceneInfo["duration"] = scene.duration;
            sceneInfo["timeOffsetInProject"] = scene.timeOffsetInProject;
            scenes.push_back(sceneInfo);
        }
        
        return scenes.dump();
    } catch (const std::exception& e) {
        return R"({"error": "Exception: )" + std::string(e.what()) + R"("})";
    }
}

#ifdef EMSCRIPTEN
// Emscripten bindings
EMSCRIPTEN_BINDINGS(pjson_editor) {
    function("initializeAPI", &initializeAPI);
    function("getProjectJson", &getProjectJson);
    function("addScene", &addScene);
    function("renameScene", &renameScene);
    function("deleteScene", &deleteScene);
    function("splitScene", &splitScene);
    function("mergeScenes", &mergeScenes);
    function("addFootage", &addFootage);
    function("getProjectStats", &getProjectStats);
    function("getSceneList", &getSceneList);
}
#endif
