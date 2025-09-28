#include "../include/pjson_editor/ExtendedAPI.h"
#include <algorithm>
#include <string>

namespace pjson {

void ExtendedDataStore::init(const ExtendedProjectAndScenesVo& initialProject) {
    project = initialProject;
}

ExtendedProjectAndScenesVo& ExtendedDataStore::getProject() { 
    return project; 
}

const ExtendedProjectAndScenesVo& ExtendedDataStore::getProject() const { 
    return project; 
}

ExtendedProjectScene* ExtendedDataStore::findScene(const std::string& sceneUuid) {
    for (auto& scene : project.scenes) {
        if (scene.uuid == sceneUuid) {
            return &scene;
        }
    }
    return nullptr;
}

void ExtendedDataStore::recomputeOffsets() {
    int totalOffset = 0;
    for (auto& scene : project.scenes) {
        scene.timeOffsetInProject = totalOffset;
        
        // Update timelines within each scene
        for (auto& timeline : scene.aRolls) {
            timeline.timeOffsetInProject = totalOffset + timeline.timeOffsetInScene;
        }
        for (auto& timeline : scene.bRolls) {
            timeline.timeOffsetInProject = totalOffset + timeline.timeOffsetInScene;
        }
        for (auto& voiceOver : scene.voiceOvers) {
            voiceOver.timeOffsetInProject = totalOffset + voiceOver.timeOffsetInProject;
        }
        
        totalOffset += scene.duration;
    }
    
    // Also update project-level timelines 
    totalOffset = 0;
    for (const auto& scene : project.scenes) {
        // Update all timelines that belong to this scene
        for (auto& timeline : project.timelines) {
            if (timeline.sceneUuid == scene.uuid) {
                timeline.timeOffsetInProject = totalOffset + timeline.timeOffsetInScene;
            }
        }
        totalOffset += scene.duration;
    }
    
    // Update BGM durations to match total project duration
    // This matches API behavior where BGM duration is automatically adjusted to project timeline
    int totalProjectDuration = totalOffset;
    for (auto& bgm : project.bgms) {
        if (bgm.duration != totalProjectDuration) {
            bgm.duration = totalProjectDuration;
        }
    }
}

// void ExtendedDataStore::insertScene(const ExtendedProjectScene& scene, int index) {
//     if (index < 0 || index >= (int)project.scenes.size()) {
//         project.scenes.push_back(scene);
//     } else {
//         project.scenes.insert(project.scenes.begin() + index, scene);
//     }
//     recomputeOffsets();
// }

// bool ExtendedDataStore::removeScene(const std::string& sceneUuid) {
//     auto it = std::find_if(project.scenes.begin(), project.scenes.end(),
//         [&sceneUuid](const ExtendedProjectScene& scene) {
//             return scene.uuid == sceneUuid;
//         });
    
//     if (it != project.scenes.end()) {
//         project.scenes.erase(it);
//         recomputeOffsets();
//         return true;
//     }
//     return false;
// }

// void ExtendedDataStore::moveScene(const std::string& sceneUuid, int newIndex) {
//     auto it = std::find_if(project.scenes.begin(), project.scenes.end(),
//         [&sceneUuid](const ExtendedProjectScene& scene) {
//             return scene.uuid == sceneUuid;
//         });
    
//     if (it != project.scenes.end()) {
//         auto scene = *it;
//         project.scenes.erase(it);
        
//         if (newIndex >= (int)project.scenes.size()) {
//             project.scenes.push_back(scene);
//         } else {
//             project.scenes.insert(project.scenes.begin() + newIndex, scene);
//         }
//         recomputeOffsets();
//     }
// }

} // namespace pjson
