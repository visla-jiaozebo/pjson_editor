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
        if (!g_dataStore->loadFromJson(projectJson)) {
            return false;
        }
        
        g_api = std::make_unique<ExtendedControllerAPI>(g_dataStore.get());
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
        return "{}";
    }
    return g_dataStore->toJson();
}

/**
 * Add footage to a scene
 * @param sceneUuid - UUID of the target scene
 * @param assetUuid - UUID of the asset to add
 * @param timeOffsetInScene - Time offset within the scene (milliseconds)
 * @param duration - Duration of the footage (milliseconds)
 * @param startTime - Start time in the asset (optional)
 * @param endTime - End time in the asset (optional)
 * @return string - JSON result containing success status and patches
 */
std::string addFootage(const std::string& sceneUuid, 
                      const std::string& assetUuid,
                      int timeOffsetInScene, 
                      int duration,
                      int startTime = 0,
                      int endTime = -1) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    ProjectSceneFootageAddReqBody reqBody;
    reqBody.sceneUuid = sceneUuid;
    reqBody.assetUuid = assetUuid;
    reqBody.timeOffsetInScene = timeOffsetInScene;
    reqBody.duration = duration;
    
    if (startTime >= 0) {
        reqBody.startTime = startTime;
    }
    if (endTime >= 0) {
        reqBody.endTime = endTime;
    }
    
    ApiResult result = g_api->addFootage(reqBody);
    
    nlohmann::json response = {
        {"success", result.success},
        {"message", result.message},
        {"patches", result.patches}
    };
    
    return response.dump();
}

/**
 * Delete footage from a scene
 * @param sceneUuid - UUID of the scene
 * @param timelineUuid - UUID of the timeline to delete
 * @return string - JSON result
 */
std::string deleteFootage(const std::string& sceneUuid, const std::string& timelineUuid) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    ProjectSceneFootageDeleteReqBody reqBody;
    reqBody.sceneUuid = sceneUuid;
    reqBody.timelineUuid = timelineUuid;
    
    ApiResult result = g_api->deleteFootage(reqBody);
    
    nlohmann::json response = {
        {"success", result.success},
        {"message", result.message},
        {"patches", result.patches}
    };
    
    return response.dump();
}

/**
 * Merge two scenes
 * @param sceneUuid1 - UUID of the first scene
 * @param sceneUuid2 - UUID of the second scene
 * @return string - JSON result
 */
std::string mergeScenes(const std::string& sceneUuid1, const std::string& sceneUuid2) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    ExtendedProjectSceneMergeReqBody reqBody;
    reqBody.sceneUuids = {sceneUuid1, sceneUuid2};
    
    ApiResult result = g_api->mergeScenes(reqBody);
    
    nlohmann::json response = {
        {"success", result.success},
        {"message", result.message},
        {"patches", result.patches}
    };
    
    return response.dump();
}

/**
 * Split a scene at a specific time
 * @param sceneUuid - UUID of the scene to split
 * @param splitTime - Time at which to split (milliseconds)
 * @return string - JSON result
 */
std::string splitScene(const std::string& sceneUuid, int splitTime) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    ExtendedProjectSceneSplitReqBody reqBody;
    reqBody.sceneUuid = sceneUuid;
    reqBody.splitTime = splitTime;
    
    ApiResult result = g_api->splitScene(reqBody);
    
    nlohmann::json response = {
        {"success", result.success},
        {"message", result.message},
        {"patches", result.patches}
    };
    
    return response.dump();
}

/**
 * Delete a scene
 * @param sceneUuid - UUID of the scene to delete
 * @return string - JSON result
 */
std::string deleteScene(const std::string& sceneUuid) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    ExtendedProjectSceneDeleteReqBody reqBody;
    reqBody.sceneUuid = sceneUuid;
    
    ApiResult result = g_api->deleteScene(reqBody);
    
    nlohmann::json response = {
        {"success", result.success},
        {"message", result.message},
        {"patches", result.patches}
    };
    
    return response.dump();
}

/**
 * Rename a scene
 * @param sceneUuid - UUID of the scene to rename
 * @param newName - New name for the scene
 * @return string - JSON result
 */
std::string renameScene(const std::string& sceneUuid, const std::string& newName) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    ExtendedProjectSceneRenameReqBody reqBody;
    reqBody.sceneUuid = sceneUuid;
    reqBody.name = newName;
    
    ApiResult result = g_api->renameScene(reqBody);
    
    nlohmann::json response = {
        {"success", result.success},
        {"message", result.message},
        {"patches", result.patches}
    };
    
    return response.dump();
}

/**
 * Add voice over to a scene
 * @param sceneUuid - UUID of the scene
 * @param assetUuid - UUID of the audio asset
 * @param timeOffsetInScene - Time offset within the scene
 * @param duration - Duration of the voice over
 * @param audioOnly - Whether this is audio-only
 * @return string - JSON result
 */
std::string addVoiceOver(const std::string& sceneUuid,
                        const std::string& assetUuid,
                        int timeOffsetInScene,
                        int duration,
                        bool audioOnly = true) {
    if (!g_api) {
        return R"({"success": false, "message": "API not initialized"})";
    }
    
    AddVoiceOverReqBody reqBody;
    reqBody.sceneUuid = sceneUuid;
    reqBody.assetUuid = assetUuid;
    reqBody.timeOffsetInScene = timeOffsetInScene;
    reqBody.duration = duration;
    reqBody.audioOnly = audioOnly;
    
    ApiResult result = g_api->addVoiceOver(reqBody);
    
    nlohmann::json response = {
        {"success", result.success},
        {"message", result.message},
        {"patches", result.patches}
    };
    
    return response.dump();
}

/**
 * Get statistics about the current project
 * @return string - JSON with project statistics
 */
std::string getProjectStats() {
    if (!g_dataStore) {
        return R"({"error": "API not initialized"})";
    }
    
    const auto& project = g_dataStore->getProject();
    
    nlohmann::json stats = {
        {"totalScenes", project.scenes.size()},
        {"totalTimelines", project.timelines.size()},
        {"projectName", project.name},
        {"projectUuid", project.uuid}
    };
    
    // Count scenes by type
    int defaultScenes = 0, introScenes = 0, outroScenes = 0, blankScenes = 0;
    for (const auto& scene : project.scenes) {
        switch (scene.sceneType) {
            case SceneTypeEnum::DEFAULT: defaultScenes++; break;
            case SceneTypeEnum::INTRO: introScenes++; break;
            case SceneTypeEnum::OUTRO: outroScenes++; break;
            case SceneTypeEnum::BLANK_SCENE: blankScenes++; break;
        }
    }
    
    stats["sceneTypes"] = {
        {"default", defaultScenes},
        {"intro", introScenes},
        {"outro", outroScenes},
        {"blank", blankScenes}
    };
    
    // Calculate total duration
    int totalDuration = 0;
    for (const auto& scene : project.scenes) {
        totalDuration += scene.duration;
    }
    stats["totalDuration"] = totalDuration;
    
    return stats.dump();
}

// Emscripten bindings
#ifdef EMSCRIPTEN
EMSCRIPTEN_BINDINGS(pjson_editor) {
    function("initializeAPI", &initializeAPI);
    function("getProjectJson", &getProjectJson);
    function("addFootage", &addFootage, allow_raw_pointers());
    function("deleteFootage", &deleteFootage);
    function("mergeScenes", &mergeScenes);
    function("splitScene", &splitScene);
    function("deleteScene", &deleteScene);
    function("renameScene", &renameScene);
    function("addVoiceOver", &addVoiceOver, allow_raw_pointers());
    function("getProjectStats", &getProjectStats);
}
#endif
