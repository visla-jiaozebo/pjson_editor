#include "pjson_editor/ExtendedAPI.h"
#include <iostream>

using namespace pjson;

int main() {
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
