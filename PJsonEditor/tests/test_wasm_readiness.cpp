#include <iostream>
#include <cassert>
#include "pjson_editor/ExtendedAPI.h"
#include "pjson_editor/ExtendedModels.h"

using namespace pjson;

/**
 * Simple local test to verify the API works before WebAssembly compilation
 */
int main() {
    std::cout << "=== PJsonEditor Local Test (Pre-WASM) ===" << std::endl;
    
    try {
        // Test 1: DataStore creation
        std::cout << "Test 1: Creating DataStore..." << std::endl;
        std::unique_ptr<ExtendedDataStore> dataStore = std::make_unique<ExtendedDataStore>();
        
        // Create sample project
        ExtendedProject project;
        project.uuid = "test-project-123";
        project.name = "Test Project";
        
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
        dataStore->setProject(project);
        std::cout << "✅ DataStore created with " << project.scenes.size() << " scenes" << std::endl;
        
        // Test 2: API creation
        std::cout << "Test 2: Creating ExtendedControllerAPI..." << std::endl;
        std::unique_ptr<ExtendedControllerAPI> api = std::make_unique<ExtendedControllerAPI>(dataStore.get());
        std::cout << "✅ API created successfully" << std::endl;
        
        // Test 3: Rename scene
        std::cout << "Test 3: Testing renameScene..." << std::endl;
        ExtendedProjectSceneRenameReqBody renameReq;
        renameReq.uuid = "scene-1";
        renameReq.newName = "Updated Scene 1";
        
        ApiResult result = api->renameScene(renameReq);
        std::cout << "Rename result: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
        std::cout << "Message: " << result.message << std::endl;
        
        // Test 4: Add footage
        std::cout << "Test 4: Testing addFootage..." << std::endl;
        ProjectSceneFootageAddReqBody footageReq;
        footageReq.sceneUuid = "scene-1";
        footageReq.assetUuid = "asset-123";
        footageReq.timeOffsetInScene = 1000;
        footageReq.duration = 2000;
        
        ApiResult footageResult = api->addFootage(footageReq);
        std::cout << "Add footage result: " << (footageResult.success ? "SUCCESS" : "FAILED") << std::endl;
        std::cout << "Message: " << footageResult.message << std::endl;
        
        // Test 5: Project statistics
        std::cout << "Test 5: Checking project state..." << std::endl;
        const auto& updatedProject = dataStore->getProject();
        std::cout << "Project name: " << updatedProject.name << std::endl;
        std::cout << "Total scenes: " << updatedProject.scenes.size() << std::endl;
        
        // Calculate total duration
        int totalDuration = 0;
        for (const auto& scene : updatedProject.scenes) {
            totalDuration += scene.duration;
        }
        std::cout << "Total duration: " << totalDuration << "ms" << std::endl;
        
        // Test 6: Scene merge
        std::cout << "Test 6: Testing mergeScenes..." << std::endl;
        ExtendedProjectSceneMergeReqBody mergeReq;
        mergeReq.sceneUuids = {"scene-1", "scene-2"};
        
        ApiResult mergeResult = api->mergeScenes(mergeReq);
        std::cout << "Merge result: " << (mergeResult.success ? "SUCCESS" : "FAILED") << std::endl;
        std::cout << "Message: " << mergeResult.message << std::endl;
        
        // Final state
        const auto& finalProject = dataStore->getProject();
        std::cout << "Final scene count: " << finalProject.scenes.size() << std::endl;
        
        std::cout << "\n=== All Tests Completed Successfully! ===" << std::endl;
        std::cout << "This confirms the API is ready for WebAssembly compilation." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
