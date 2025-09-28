#include <iostream>
#include <cassert>
#include "../include/pjson_editor/ExtendedAPI.h"

using namespace pjson;

void testMergeScenesBusinessLogic() {
    std::cout << "=== Testing mergeScenes Business Logic Implementation ===" << std::endl;
    
    // Create a simple test scenario
    ExtendedProjectAndScenesVo project;
    project.projectUuid = "test-project";
    
    // Scene 1
    ExtendedProjectScene scene1;
    scene1.uuid = "scene-1";
    scene1.name = "First Scene";
    scene1.duration = 5000;
    scene1.timeOffsetInProject = 0;
    scene1.pauseTime = 0;
    scene1.sceneType = SceneTypeEnum::DEFAULT;
    
    // Scene 2  
    ExtendedProjectScene scene2;
    scene2.uuid = "scene-2";
    scene2.name = "Second Scene";
    scene2.duration = 3000;
    scene2.timeOffsetInProject = 5000;
    scene2.pauseTime = 500; // 500ms pause
    scene2.sceneType = SceneTypeEnum::DEFAULT;
    
    project.scenes = {scene1, scene2};
    
    // Setup API and DataStore
    ExtendedDataStore dataStore;
    dataStore.init(project);
    
    ExtendedControllerAPI api;
    api.setDataStore(&dataStore);
    
    // Test merge request
    ExtendedProjectSceneMergeReqBody reqBody;
    reqBody.sceneUuids = {"scene-1", "scene-2"};
    
    // Execute merge
    ApiResult result = api.mergeScenes(reqBody);
    
    // Verify results
    std::cout << "Merge result: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Message: " << result.message << std::endl;
    
    if (result.success) {
        // Check if we have exactly one scene left
        auto& scenes = dataStore.getProject().scenes;
        std::cout << "Scenes after merge: " << scenes.size() << std::endl;
        
        if (!scenes.empty()) {
            auto& mergedScene = scenes[0];
            std::cout << "Merged scene name: " << mergedScene.name << std::endl;
            std::cout << "Merged scene duration: " << mergedScene.duration << std::endl;
            std::cout << "Expected duration: " << (5000 + 3000 - 500) << std::endl;
            
            // Verify business logic
            bool nameCorrect = mergedScene.name.find("Merged") != std::string::npos;
            bool durationCorrect = (mergedScene.duration == 7500); // 5000 + 3000 - 500
            
            std::cout << "✓ Name contains 'Merged': " << (nameCorrect ? "YES" : "NO") << std::endl;
            std::cout << "✓ Duration calculation: " << (durationCorrect ? "CORRECT" : "INCORRECT") << std::endl;
            
            if (nameCorrect && durationCorrect) {
                std::cout << "🎉 MERGE SCENES BUSINESS LOGIC PARITY ACHIEVED! 🎉" << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "Testing mergeScenes with Complete Business Logic Parity" << std::endl;
    std::cout << "========================================================" << std::endl << std::endl;
    
    try {
        testMergeScenesBusinessLogic();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
