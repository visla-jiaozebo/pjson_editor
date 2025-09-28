#include "pjson_editor/ExtendedAPI.h"
#include <iostream>

int main() {
    using namespace pjson;
    
    // Test basic instantiation
    ExtendedProjectAndScenesVo project;
    project.projectUuid = "test-project";
    project.status = StatusEnum::ACTIVE;
    
    ExtendedProjectScene scene;
    scene.uuid = "test-scene";
    scene.name = "Test Scene";
    scene.sceneType = SceneTypeEnum::DEFAULT;
    scene.duration = 5000;
    
    project.scenes.push_back(scene);
    
    std::cout << "ExtendedModels test passed - project has " 
              << project.scenes.size() << " scenes" << std::endl;
    
    return 0;
}
