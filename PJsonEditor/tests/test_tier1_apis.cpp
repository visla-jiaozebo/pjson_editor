#include "pjson_editor/ExtendedAPI.h"
#include <iostream>

using namespace pjson;

int main() {
    ExtendedControllerAPI api;
    
    std::cout << "=== Testing Tier 1 High-Priority APIs ===" << std::endl;
    
    // Test 1: Add footage
    ProjectSceneFootageAddReqBody addFootageReq;
    addFootageReq.sceneUuid = "scene-1";
    addFootageReq.assetUuid = "asset-video-1";
    addFootageReq.timeOffsetInScene = 1000;
    addFootageReq.duration = 3000;
    addFootageReq.startTime = 5000;
    addFootageReq.endTime = 8000;
    
    auto result1 = api.addFootage(addFootageReq);
    std::cout << "1. Add footage: " << (result1.success ? "SUCCESS" : "FAILED") 
              << " - " << result1.message << std::endl;
    
    // Test 2: Delete footage
    ProjectSceneFootageDeleteReqBody deleteFootageReq;
    deleteFootageReq.sceneUuid = "scene-1";
    deleteFootageReq.timelineUuid = "timeline-footage-1";
    
    auto result2 = api.deleteFootage(deleteFootageReq);
    std::cout << "2. Delete footage: " << (result2.success ? "SUCCESS" : "FAILED") 
              << " - " << result2.message << std::endl;
    
    // Test 3: Add voice over
    AddVoiceOverReqBody addVoiceReq;
    addVoiceReq.sceneUuid = "scene-1";
    addVoiceReq.assetUuid = "asset-audio-1";
    addVoiceReq.timeOffsetInScene = 0;
    addVoiceReq.duration = 5000;
    addVoiceReq.audioOnly = false;
    
    auto result3 = api.addVoiceOver(addVoiceReq);
    std::cout << "3. Add voice over: " << (result3.success ? "SUCCESS" : "FAILED") 
              << " - " << result3.message << std::endl;
    
    // Test 4: Adjust voice over
    AdjustVoiceOverReqBody adjustVoiceReq;
    adjustVoiceReq.sceneUuid = "scene-1";
    adjustVoiceReq.timelineUuid = "timeline-voice-1";
    adjustVoiceReq.volume = 0.8;
    adjustVoiceReq.duration = 4000;
    
    auto result4 = api.adjustVoiceOver(adjustVoiceReq);
    std::cout << "4. Adjust voice over: " << (result4.success ? "SUCCESS" : "FAILED") 
              << " - " << result4.message << std::endl;
    
    // Test 5: Edit script
    ProjectSceneEditScriptReqBody editScriptReq;
    editScriptReq.sceneUuid = "scene-1";
    editScriptReq.script.text = "This is the updated script for the scene.";
    editScriptReq.script.modified = true;
    
    auto result5 = api.editScript(editScriptReq);
    std::cout << "5. Edit script: " << (result5.success ? "SUCCESS" : "FAILED") 
              << " - " << result5.message << std::endl;
    
    // Test 6: Change highlights
    EditSceneHighLightReqBody highlightReq;
    highlightReq.sceneUuid = "scene-1";
    highlightReq.highLights = {"important", "key", "highlight"};
    
    auto result6 = api.changeHighlight(highlightReq);
    std::cout << "6. Change highlight: " << (result6.success ? "SUCCESS" : "FAILED") 
              << " - " << result6.message << std::endl;
    
    // Test 7: Delete transition
    auto result7 = api.deleteTransition("scene-1");
    std::cout << "7. Delete transition: " << (result7.success ? "SUCCESS" : "FAILED") 
              << " - " << result7.message << std::endl;
    
    // Test 8: Set main story order
    SetMainStoryOrderReqBody orderReq;
    orderReq.timelineUuids = {"timeline-1", "timeline-3", "timeline-2"};
    
    auto result8 = api.setMainStoryOrder(orderReq);
    std::cout << "8. Set main story order: " << (result8.success ? "SUCCESS" : "FAILED") 
              << " - " << result8.message << std::endl;
    
    // Test 9: Change fit type
    ChangeFitTypeReqBody fitTypeReq;
    fitTypeReq.fitType = 1; // fill
    
    auto result9 = api.changeFitType(fitTypeReq);
    std::cout << "9. Change fit type: " << (result9.success ? "SUCCESS" : "FAILED") 
              << " - " << result9.message << std::endl;
    
    // Test 10: Change scale to all
    UpdateProjectScaleReqBody scaleReq;
    scaleReq.timelineUuid = "timeline-1";
    scaleReq.scale.scaleX = 1.2;
    scaleReq.scale.scaleY = 1.2;
    scaleReq.scale.offsetX = 0.1;
    scaleReq.scale.offsetY = 0.1;
    
    auto result10 = api.changeScaleToAll(scaleReq);
    std::cout << "10. Change scale to all: " << (result10.success ? "SUCCESS" : "FAILED") 
              << " - " << result10.message << std::endl;
    
    std::cout << "\n=== All Tier 1 APIs tested successfully! ===" << std::endl;
    
    return 0;
}
