#include <iostream>
#include <string>
#include <memory>
#include "pjson_editor/ExtendedAPI.h"
#include "pjson_editor/ExtendedModels.h"

using namespace pjson;

/**
 * Simple test for WebAssembly demonstration
 * This file can be compiled to WebAssembly to demonstrate the API
 */

// Simple wrapper functions for WebAssembly
extern "C" {
    
// Global API instance pointer
static ExtendedControllerAPI* g_api = nullptr;
static ExtendedDataStore* g_dataStore = nullptr;

/**
 * Initialize the API with sample data
 */
int initializeSampleProject() {
    try {
        // Clean up any existing instances
        if (g_api) {
            delete g_api;
            g_api = nullptr;
        }
        if (g_dataStore) {
            delete g_dataStore;
            g_dataStore = nullptr;
        }
        
        // Create new instances
        g_dataStore = new ExtendedDataStore();
        
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
        
        g_api = new ExtendedControllerAPI(g_dataStore);
        return 1; // Success
    } catch (...) {
        return 0; // Failure
    }
}

/**
 * Get the number of scenes in the project
 */
int getSceneCount() {
    if (!g_dataStore) return -1;
    
    const auto& project = g_dataStore->getProject();
    return static_cast<int>(project.scenes.size());
}

/**
 * Rename a scene (simplified version)
 */
int renameScene(const char* sceneUuid, const char* newName) {
    if (!g_api || !sceneUuid || !newName) return 0;
    
    try {
        ExtendedProjectSceneRenameReqBody reqBody;
        reqBody.sceneUuid = std::string(sceneUuid);
        reqBody.name = std::string(newName);
        
        ApiResult result = g_api->renameScene(reqBody);
        return result.success ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

/**
 * Get project name (simplified)
 */
const char* getProjectName() {
    if (!g_dataStore) return "Not initialized";
    
    static std::string name = g_dataStore->getProject().name;
    return name.c_str();
}

/**
 * Clean up resources
 */
void cleanup() {
    if (g_api) {
        delete g_api;
        g_api = nullptr;
    }
    if (g_dataStore) {
        delete g_dataStore;
        g_dataStore = nullptr;
    }
}

} // extern "C"

// Native test main function
int main() {
    std::cout << "=== PJsonEditor WebAssembly Test ===" << std::endl;
    
    // Test initialization
    std::cout << "Initializing sample project..." << std::endl;
    if (initializeSampleProject()) {
        std::cout << "✅ Initialization successful" << std::endl;
    } else {
        std::cout << "❌ Initialization failed" << std::endl;
        return 1;
    }
    
    // Test getting project info
    std::cout << "Project name: " << getProjectName() << std::endl;
    std::cout << "Scene count: " << getSceneCount() << std::endl;
    
    // Test renaming a scene
    std::cout << "Renaming scene-1..." << std::endl;
    if (renameScene("scene-1", "Updated Scene 1")) {
        std::cout << "✅ Scene renamed successfully" << std::endl;
    } else {
        std::cout << "❌ Scene rename failed" << std::endl;
    }
    
    // Test getting updated project info
    std::cout << "Final scene count: " << getSceneCount() << std::endl;
    
    // Cleanup
    cleanup();
    std::cout << "✅ Test completed successfully" << std::endl;
    
    return 0;
}
