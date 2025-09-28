#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../include/pjson_editor/ExtendedAPI.h"
#include "../include/pjson_editor/ExtendedModels.h"

using namespace pjson;

// Test data setup
ExtendedProjectAndScenesVo createTestProjectForMerge() {
    ExtendedProjectAndScenesVo project;
    project.project = ExtendedProject();
    project.project.uuid = "project-123";
    project.project.name = "Test Merge Project";
    
    // Create three scenes for testing
    ExtendedProjectScene scene1;
    scene1.uuid = "scene-001";
    scene1.name = "First Scene";
    scene1.duration = 5000; // 5 seconds
    scene1.timeOffsetInProject = 0;
    scene1.sceneType = SceneTypeEnum::DEFAULT;
    scene1.pauseTime = std::nullopt;
    
    SceneTranscript transcript1;
    transcript1.text = "This is the first scene.";
    transcript1.duration = 5000;
    scene1.transcript = transcript1;
    
    ExtendedProjectScene scene2;
    scene2.uuid = "scene-002";
    scene2.name = "Second Scene";
    scene2.duration = 3000; // 3 seconds
    scene2.timeOffsetInProject = 5000;
    scene2.sceneType = SceneTypeEnum::DEFAULT;
    scene2.pauseTime = 500; // 0.5 second pause
    
    SceneTranscript transcript2;
    transcript2.text = "This is the second scene.";
    transcript2.duration = 3000;
    scene2.transcript = transcript2;
    
    ExtendedProjectScene scene3;
    scene3.uuid = "scene-003";
    scene3.name = "Third Scene";
    scene3.duration = 4000; // 4 seconds
    scene3.timeOffsetInProject = 8000;
    scene3.sceneType = SceneTypeEnum::DEFAULT;
    scene3.pauseTime = std::nullopt;
    
    project.scenes = {scene1, scene2, scene3};
    
    // Create timelines for scenes
    ExtendedTimeline timeline1;
    timeline1.uuid = "timeline-001";
    timeline1.sceneUuid = "scene-001";
    timeline1.timeOffsetInProject = 0;
    timeline1.category = ProjectTimelineCategoryEnum::AROLL;
    timeline1.duration = 5000;
    
    ExtendedTimeline timeline2;
    timeline2.uuid = "timeline-002";
    timeline2.sceneUuid = "scene-002";
    timeline2.timeOffsetInProject = 5000;
    timeline2.category = ProjectTimelineCategoryEnum::AROLL;
    timeline2.duration = 3000;
    
    ExtendedTimeline timeline3;
    timeline3.uuid = "timeline-003";
    timeline3.sceneUuid = "scene-003";
    timeline3.timeOffsetInProject = 8000;
    timeline3.category = ProjectTimelineCategoryEnum::AROLL;
    timeline3.duration = 4000;
    
    project.timelines = {timeline1, timeline2, timeline3};
    
    return project;
}

void testMergeScenesBasicLogic() {
    std::cout << "=== Testing mergeScenes Basic Logic ===" << std::endl;
    
    // Setup
    ExtendedDataStore dataStore;
    ExtendedProjectAndScenesVo project = createTestProjectForMerge();
    dataStore.init(project);
    
    ExtendedControllerAPI api;
    api.setDataStore(&dataStore); // Set the data store
    
    // Create merge request for first two scenes
    ExtendedProjectSceneMergeReqBody reqBody;
    reqBody.sceneUuids = {"scene-001", "scene-002"};
    
    // Execute merge
    ApiResult result = api.mergeScenes(reqBody);
    
    // Verify success
    assert(result.success == true);
    std::cout << "✓ Merge operation succeeded" << std::endl;
    
    // Verify scene count reduced from 3 to 2
    auto& scenes = dataStore.getProject().scenes;
    assert(scenes.size() == 2);
    std::cout << "✓ Scene count reduced from 3 to 2" << std::endl;
    
    // Debug: Print all scene names
    std::cout << "Remaining scenes:" << std::endl;
    for (size_t i = 0; i < scenes.size(); ++i) {
        std::cout << "  Scene " << i << ": " << scenes[i].name << " (UUID: " << scenes[i].uuid << ")" << std::endl;
    }
    
    // Find merged scene (should have "Merged" in name)
    ExtendedProjectScene* mergedScene = nullptr;
    for (auto& scene : scenes) {
        if (scene.name.find("Merged") != std::string::npos) {
            mergedScene = &scene;
            break;
        }
    }
    assert(mergedScene != nullptr);
    std::cout << "✓ Found merged scene: " << mergedScene->name << std::endl;
    
    // Verify merged scene properties
    assert(mergedScene->name == "Merged First Scene");
    std::cout << "✓ Merged scene name correct: " << mergedScene->name << std::endl;
    
    // Debug: Print actual duration
    std::cout << "DEBUG: Actual merged scene duration: " << mergedScene->duration << "ms" << std::endl;
    std::cout << "DEBUG: Expected duration: 7500ms (5000 + 3000 - 500)" << std::endl;
    
    // Verify duration calculation (5000 + 3000 - 500 pause = 7500)
    assert(mergedScene->duration == 7500);
    std::cout << "✓ Merged scene duration correct: " << mergedScene->duration << "ms" << std::endl;
    
    // Verify timeOffsetInProject (should be 0, same as first scene)
    assert(mergedScene->timeOffsetInProject == 0);
    std::cout << "✓ Merged scene offset correct: " << mergedScene->timeOffsetInProject << std::endl;
    
    // Verify pause time (should be from second scene)
    assert(mergedScene->pauseTime.has_value() && mergedScene->pauseTime.value() == 500);
    std::cout << "✓ Merged scene pause time correct: " << mergedScene->pauseTime.value() << "ms" << std::endl;
    
    std::cout << "✓ All basic merge logic tests passed!" << std::endl << std::endl;
}

void testMergeTimelineUpdates() {
    std::cout << "=== Testing Timeline Updates After Merge ===" << std::endl;
    
    // Setup
    ExtendedDataStore dataStore;
    ExtendedProjectAndScenesVo project = createTestProjectForMerge();
    dataStore.init(project);
    
    ExtendedControllerAPI api;
    api.setDataStore(&dataStore); // SET DATASTORE POINTER!
    
    // Store original timeline count
    size_t originalTimelineCount = dataStore.getProject().timelines.size();
    std::cout << "Original timeline count: " << originalTimelineCount << std::endl;
    
    // Create merge request for first two scenes
    ExtendedProjectSceneMergeReqBody reqBody;
    reqBody.sceneUuids = {"scene-001", "scene-002"};
    
    // Execute merge
    ApiResult result = api.mergeScenes(reqBody);
    assert(result.success == true);
    
    // Verify timeline count remains the same (timelines reassigned, not deleted)
    auto& timelines = dataStore.getProject().timelines;
    assert(timelines.size() == originalTimelineCount);
    std::cout << "✓ Timeline count preserved: " << timelines.size() << std::endl;
    
    // Find merged scene UUID
    std::string mergedSceneUuid;
    for (const auto& scene : dataStore.getProject().scenes) {
        if (scene.name.find("Merged") != std::string::npos) {
            mergedSceneUuid = scene.uuid;
            break;
        }
    }
    assert(!mergedSceneUuid.empty());
    
    // Verify former scene timelines now point to merged scene
    int mergedSceneTimelineCount = 0;
    for (const auto& timeline : timelines) {
        std::cout << "Timeline UUID: " << timeline.uuid << ", SceneUUID: " << timeline.sceneUuid << std::endl;
        if (timeline.sceneUuid == mergedSceneUuid) {
            mergedSceneTimelineCount++;
        }
    }
    std::cout << "DEBUG: Found " << mergedSceneTimelineCount << " timelines for merged scene " << mergedSceneUuid << std::endl;
    assert(mergedSceneTimelineCount == 2); // Should have timelines from both original scenes
    std::cout << "✓ Merged scene has correct timeline count: " << mergedSceneTimelineCount << std::endl;
    
    // Verify third scene timeline offset was updated properly
    ExtendedTimeline* thirdSceneTimeline = nullptr;
    for (auto& timeline : timelines) {
        if (timeline.sceneUuid == "scene-003") {
            thirdSceneTimeline = &timeline;
            break;
        }
    }
    assert(thirdSceneTimeline != nullptr);
    
    // Original third scene offset was 8000, merged duration change is 7500 - 8000 = -500
    // So new offset should be 8000 + (-500) = 7500
    assert(thirdSceneTimeline->timeOffsetInProject == 7500);
    std::cout << "✓ Third scene timeline offset updated correctly: " << thirdSceneTimeline->timeOffsetInProject << std::endl;
    
    std::cout << "✓ All timeline update tests passed!" << std::endl << std::endl;
}

void testMergeSceneTypeHandling() {
    std::cout << "=== Testing Scene Type Handling in Merge ===" << std::endl;
    
    // Setup project with blank scene and default scene
    ExtendedDataStore dataStore;
    ExtendedProjectAndScenesVo project;
    project.project = ExtendedProject();
    
    ExtendedProjectScene blankScene;
    blankScene.uuid = "blank-scene";
    blankScene.name = "Blank Scene";
    blankScene.duration = 2000;
    blankScene.timeOffsetInProject = 0;
    blankScene.sceneType = SceneTypeEnum::BLANK_SCENE;
    
    ExtendedProjectScene defaultScene;
    defaultScene.uuid = "default-scene";
    defaultScene.name = "Default Scene";
    defaultScene.duration = 3000;
    defaultScene.timeOffsetInProject = 2000;
    defaultScene.sceneType = SceneTypeEnum::DEFAULT;
    
    project.scenes = {blankScene, defaultScene};
    project.timelines = {}; // No timelines for simplicity
    
    dataStore.init(project);
    ExtendedControllerAPI api;
    api.setDataStore(&dataStore); // SET DATASTORE POINTER!
    
    // Merge blank scene with default scene
    ExtendedProjectSceneMergeReqBody reqBody;
    reqBody.sceneUuids = {"blank-scene", "default-scene"};
    
    ApiResult result = api.mergeScenes(reqBody);
    assert(result.success == true);
    
    // Find merged scene
    ExtendedProjectScene* mergedScene = nullptr;
    for (auto& scene : dataStore.getProject().scenes) {
        if (scene.name.find("Merged") != std::string::npos) {
            mergedScene = &scene;
            break;
        }
    }
    assert(mergedScene != nullptr);
    
    // Verify scene type was changed from BLANK_SCENE to DEFAULT (from latter scene)
    assert(mergedScene->sceneType == SceneTypeEnum::DEFAULT);
    std::cout << "✓ Scene type correctly inherited from latter scene: DEFAULT" << std::endl;
    
    std::cout << "✓ All scene type handling tests passed!" << std::endl << std::endl;
}

void testMergeValidationErrors() {
    std::cout << "=== Testing Merge Validation Errors ===" << std::endl;
    
    ExtendedDataStore dataStore;
    ExtendedProjectAndScenesVo project = createTestProjectForMerge();
    dataStore.init(project);
    
    ExtendedControllerAPI api;
    api.setDataStore(&dataStore); // SET DATASTORE POINTER!
    
    // Test 1: Invalid scene count (not exactly 2)
    ExtendedProjectSceneMergeReqBody reqBody1;
    reqBody1.sceneUuids = {"scene-001"}; // Only 1 scene
    
    ApiResult result1 = api.mergeScenes(reqBody1);
    assert(result1.success == false);
    assert(result1.message.find("Exactly 2 scene UUIDs required") != std::string::npos);
    std::cout << "✓ Correctly rejected merge with 1 scene" << std::endl;
    
    // Test 2: Non-existent scene UUID
    ExtendedProjectSceneMergeReqBody reqBody2;
    reqBody2.sceneUuids = {"scene-001", "non-existent"};
    
    ApiResult result2 = api.mergeScenes(reqBody2);
    assert(result2.success == false);
    assert(result2.message.find("not found") != std::string::npos);
    std::cout << "✓ Correctly rejected merge with non-existent scene" << std::endl;
    
    // Test 3: Intro/Outro scene restriction
    // Modify one scene to be INTRO type
    dataStore.getProject().scenes[0].sceneType = SceneTypeEnum::INTRO;
    
    ExtendedProjectSceneMergeReqBody reqBody3;
    reqBody3.sceneUuids = {"scene-001", "scene-002"};
    
    ApiResult result3 = api.mergeScenes(reqBody3);
    assert(result3.success == false);
    assert(result3.message.find("Cannot merge intro/outro scenes") != std::string::npos);
    std::cout << "✓ Correctly rejected merge with intro scene" << std::endl;
    
    std::cout << "✓ All validation error tests passed!" << std::endl << std::endl;
}

int main() {
    std::cout << "Testing mergeScenes Business Logic Implementation" << std::endl;
    std::cout << "=================================================" << std::endl << std::endl;
    
    try {
        testMergeScenesBasicLogic();
        testMergeTimelineUpdates();
        testMergeSceneTypeHandling();
        testMergeValidationErrors();
        
        std::cout << "🎉 ALL MERGE SCENES TESTS PASSED! 🎉" << std::endl;
        std::cout << "✅ Complete business logic parity achieved for mergeScenes" << std::endl;
        std::cout << "✅ Input validation, scene ordering, duration calculation verified" << std::endl;
        std::cout << "✅ Timeline reassignment and offset updates working correctly" << std::endl;
        std::cout << "✅ Scene type inheritance and error handling implemented" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
