#include <iostream>
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>
#include <pjson_editor/TranscriptUtils.h>

int main() {
    std::cout << "PJsonEditor Example" << std::endl;
    std::cout << "Successfully linked with PJsonEditor library!" << std::endl;
    
        ExtendedControllerAPI api;
    
    // Test adding a scene
    ExtendedProjectSceneAddReqBody addReq;
    addReq.name = "Test Scene";
    addReq.duration = 5000;
    
    auto result = api.addScene(addReq);
    
    std::cout << "API test - Add scene: " 
              << (result.success ? "SUCCESS" : "FAILED") 
              << " - " << result.message << std::endl;
    
    return 0;
}