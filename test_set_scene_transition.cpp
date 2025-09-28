#include "PJsonEditor/include/pjson_editor/ExtendedAPI.h"
#include <iostream>

int main() {
    // Test that setSceneTransition method exists and can be called
    pjson::ExtendedControllerAPI api;
    pjson::ProjectSceneTransitionReqBody reqBody;
    
    // Set up test parameters to match API
    reqBody.sceneUuid = "test-scene-123";
    reqBody.projectUuid = "test-project-456";
    reqBody.type = "fadeIn";
    reqBody.duration = 1000;
    reqBody.forAllScenes = false;
    
    std::cout << "Testing setSceneTransition method..." << std::endl;
    std::cout << "Request parameters:" << std::endl;
    std::cout << "  sceneUuid: " << reqBody.sceneUuid << std::endl;
    std::cout << "  projectUuid: " << reqBody.projectUuid << std::endl;
    std::cout << "  type: " << reqBody.type << std::endl;
    std::cout << "  duration: " << reqBody.duration << std::endl;
    std::cout << "  forAllScenes: " << (reqBody.forAllScenes ? "true" : "false") << std::endl;
    
    // This would normally fail because no dataStore is initialized,
    // but it proves the method signature is correct
    auto result = api.setSceneTransition(reqBody);
    
    std::cout << "setSceneTransition method called successfully!" << std::endl;
    std::cout << "Result success: " << (result.success ? "true" : "false") << std::endl;
    std::cout << "Result message: " << result.message << std::endl;
    
    return 0;
}