#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>

using namespace pjson;

class AddSceneTestFixture {
public:
    ExtendedDataStore dataStore;
    ExtendedControllerAPI api;
    
    AddSceneTestFixture() {
        // Initialize the data store with an empty project
        ExtendedProjectAndScenesVo emptyProject;
        emptyProject.projectUuid = "test-project-uuid";
        emptyProject.project.uuid = "test-project-uuid"; 
        emptyProject.project.name = "Test Project";
        emptyProject.scenes = std::vector<ExtendedProjectScene>();
        
        dataStore.init(emptyProject);
        api.setDataStore(&dataStore);
    }
};

TEST_CASE_FIXTURE(AddSceneTestFixture, "AddScene - Add first scene") {
    // Arrange
    ExtendedProjectSceneAddReqBody reqBody;
    reqBody.addPosition = 0;  // Add at position 0
    reqBody.duration = 5000;  // 5 seconds
    reqBody.name = "Test Scene 1";
    reqBody.sceneType = SceneTypeEnum::DEFAULT;
    
    // Act
    ApiResult result = api.addScene(reqBody);
    
    // Assert
    CHECK(result.success);
    CHECK_FALSE(result.message.empty());
    CHECK_FALSE(result.patch.empty());
    
    // Verify the scene was actually added
    const auto& project = dataStore.getProject();
    CHECK(project.scenes.size() == 1);
    CHECK(project.scenes[0].name == "Test Scene 1");
    CHECK(project.scenes[0].sceneType == SceneTypeEnum::DEFAULT);
    CHECK(project.scenes[0].duration == 5000);
}

TEST_CASE_FIXTURE(AddSceneTestFixture, "AddScene - Add multiple scenes") {
    // Arrange - Add first scene
    ExtendedProjectSceneAddReqBody reqBody1;
    reqBody1.addPosition = 0;
    reqBody1.duration = 3000;
    reqBody1.name = "Scene 1";
    reqBody1.sceneType = SceneTypeEnum::DEFAULT;
    
    ExtendedProjectSceneAddReqBody reqBody2;
    reqBody2.addPosition = 1;
    reqBody2.duration = 4000;
    reqBody2.name = "Scene 2";
    reqBody2.sceneType = SceneTypeEnum::DEFAULT;
    
    // Act
    ApiResult result1 = api.addScene(reqBody1);
    ApiResult result2 = api.addScene(reqBody2);
    
    // Assert
    CHECK(result1.success);
    CHECK(result2.success);
    
    // Verify both scenes were added
    const auto& project = dataStore.getProject();
    CHECK(project.scenes.size() == 2);
    CHECK(project.scenes[0].name == "Scene 1");
    CHECK(project.scenes[1].name == "Scene 2");
}

TEST_CASE_FIXTURE(AddSceneTestFixture, "AddScene - Add scene with different types") {
    SUBCASE("Add INTRO scene") {
        ExtendedProjectSceneAddReqBody reqBody;
        reqBody.addPosition = 0;
        reqBody.duration = 2000;
        reqBody.name = "Intro Scene";
        reqBody.sceneType = SceneTypeEnum::INTRO;
        
        ApiResult result = api.addScene(reqBody);
        
        CHECK(result.success);
        const auto& project = dataStore.getProject();
        CHECK(project.scenes.size() == 1);
        CHECK(project.scenes[0].sceneType == SceneTypeEnum::INTRO);
        CHECK(project.scenes[0].name == "Intro Scene");
    }
    
    SUBCASE("Add OUTRO scene") {
        ExtendedProjectSceneAddReqBody reqBody;
        reqBody.addPosition = 0;
        reqBody.duration = 3000;
        reqBody.name = "Outro Scene";
        reqBody.sceneType = SceneTypeEnum::OUTRO;
        
        ApiResult result = api.addScene(reqBody);
        
        CHECK(result.success);
        const auto& project = dataStore.getProject();
        CHECK(project.scenes.size() == 1);
        CHECK(project.scenes[0].sceneType == SceneTypeEnum::OUTRO);
        CHECK(project.scenes[0].name == "Outro Scene");
    }
    
    SUBCASE("Add BLANK_SCENE") {
        ExtendedProjectSceneAddReqBody reqBody;
        reqBody.addPosition = 0;
        reqBody.duration = 1000;
        reqBody.name = "Blank Scene";
        reqBody.sceneType = SceneTypeEnum::BLANK_SCENE;
        
        ApiResult result = api.addScene(reqBody);
        
        CHECK(result.success);
        const auto& project = dataStore.getProject();
        CHECK(project.scenes.size() == 1);
        CHECK(project.scenes[0].sceneType == SceneTypeEnum::BLANK_SCENE);
        CHECK(project.scenes[0].name == "Blank Scene");
    }
}

TEST_CASE_FIXTURE(AddSceneTestFixture, "AddScene - Add scene without optional fields") {
    // Arrange - Only required fields
    ExtendedProjectSceneAddReqBody reqBody;
    reqBody.addPosition = 0;
    // duration is optional
    // name is optional
    // sceneType is optional (should default)
    
    // Act
    ApiResult result = api.addScene(reqBody);
    
    // Assert
    CHECK(result.success);
    
    // Verify scene was added with defaults
    const auto& project = dataStore.getProject();
    CHECK(project.scenes.size() == 1);
    CHECK(project.scenes[0].sceneType == SceneTypeEnum::DEFAULT);  // Should use default
}

TEST_CASE_FIXTURE(AddSceneTestFixture, "AddScene - Verify JSON patch structure") {
    // Arrange
    ExtendedProjectSceneAddReqBody reqBody;
    reqBody.addPosition = 0;
    reqBody.duration = 5000;
    reqBody.name = "Test Scene";
    reqBody.sceneType = SceneTypeEnum::DEFAULT;
    
    // Act
    ApiResult result = api.addScene(reqBody);
    
    // Assert
    CHECK(result.success);
    CHECK(result.patch.is_array());
    
    // Verify patch contains expected operations
    bool foundAddOperation = false;
    for (const auto& operation : result.patch) {
        if (operation.contains("op") && operation["op"] == "add") {
            foundAddOperation = true;
            CHECK(operation.contains("path"));
            CHECK(operation.contains("value"));
        }
    }
    CHECK(foundAddOperation);
}

// Test fixture with pre-existing scenes
class AddSceneWithExistingScenesFixture {
public:
    ExtendedDataStore dataStore;
    ExtendedControllerAPI api;
    
    AddSceneWithExistingScenesFixture() {
        // Initialize project with existing scenes
        ExtendedProjectAndScenesVo project;
        project.projectUuid = "test-project-uuid";
        project.project.uuid = "test-project-uuid";
        project.project.name = "Test Project";
        
        // Add some existing scenes
        ExtendedProjectScene scene1;
        scene1.uuid = "existing-scene-1";
        scene1.name = "Existing Scene 1";
        scene1.duration = 3000;
        scene1.timeOffsetInProject = 0;
        scene1.sceneType = SceneTypeEnum::DEFAULT;
        
        ExtendedProjectScene scene2;
        scene2.uuid = "existing-scene-2";
        scene2.name = "Existing Scene 2";
        scene2.duration = 4000;
        scene2.timeOffsetInProject = 3000;
        scene2.sceneType = SceneTypeEnum::DEFAULT;
        
        project.scenes = {scene1, scene2};
        
        dataStore.init(project);
        api.setDataStore(&dataStore);
    }
};

TEST_CASE_FIXTURE(AddSceneWithExistingScenesFixture, "AddScene - Add to existing project") {
    // Arrange
    ExtendedProjectSceneAddReqBody reqBody;
    reqBody.addPosition = 2;  // Add at the end
    reqBody.duration = 2000;
    reqBody.name = "New Scene";
    reqBody.sceneType = SceneTypeEnum::DEFAULT;
    
    // Act
    ApiResult result = api.addScene(reqBody);
    
    // Assert
    CHECK(result.success);
    
    // Verify the new scene was added
    const auto& project = dataStore.getProject();
    CHECK(project.scenes.size() == 3);
    
    // Check that the new scene is at the correct position
    CHECK(project.scenes[2].name == "New Scene");
    CHECK(project.scenes[2].duration == 2000);
}

TEST_CASE_FIXTURE(AddSceneWithExistingScenesFixture, "AddScene - Insert in middle") {
    // Arrange
    ExtendedProjectSceneAddReqBody reqBody;
    reqBody.addPosition = 1;  // Insert between existing scenes
    reqBody.duration = 1500;
    reqBody.name = "Middle Scene";
    reqBody.sceneType = SceneTypeEnum::DEFAULT;
    
    // Act
    ApiResult result = api.addScene(reqBody);
    
    // Assert
    CHECK(result.success);
    
    // Verify the scene was inserted correctly
    const auto& project = dataStore.getProject();
    CHECK(project.scenes.size() == 3);
    CHECK(project.scenes[0].name == "Existing Scene 1");
    CHECK(project.scenes[1].name == "Middle Scene");
    CHECK(project.scenes[2].name == "Existing Scene 2");
}

TEST_CASE("AddScene - Test with null dataStore") {
    // Arrange
    ExtendedControllerAPI api;
    // Don't set dataStore (should be nullptr)
    
    ExtendedProjectSceneAddReqBody reqBody;
    reqBody.addPosition = 0;
    reqBody.name = "Test Scene";
    
    // Act & Assert
    // This should either fail gracefully or handle the null dataStore
    ApiResult result = api.addScene(reqBody);
    CHECK_FALSE(result.success);  // Should fail due to null dataStore
}