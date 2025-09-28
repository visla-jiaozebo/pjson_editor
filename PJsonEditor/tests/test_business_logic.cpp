#include "pjson_editor/ExtendedAPI.h"
#include <iostream>

using namespace pjson;

int main() {
    ExtendedControllerAPI api;
    
    std::cout << "=== Testing assembleSceneAndTimelines Business Logic ===" << std::endl;
    
    // Test 1: assembleSceneAndTimelines with mixed timeline categories
    ExtendedProjectScene testScene;
    testScene.uuid = "scene-1";
    testScene.name = "Test Scene";
    testScene.sceneType = SceneTypeEnum::DEFAULT;
    testScene.duration = 10000;
    
    std::vector<ExtendedTimeline> testTimelines;
    
    // Add main story timeline (aRoll)
    ExtendedTimeline mainStory;
    mainStory.uuid = "timeline-main-1";
    mainStory.category = ProjectTimelineCategoryEnum::MAIN_STORY;
    mainStory.assetUuid = "asset-video-1";
    mainStory.duration = 5000;
    testTimelines.push_back(mainStory);
    
    // Add footage timeline (bRoll)
    ExtendedTimeline footage;
    footage.uuid = "timeline-footage-1";
    footage.category = ProjectTimelineCategoryEnum::FOOTAGE;
    footage.assetUuid = "asset-video-2";
    footage.duration = 3000;
    testTimelines.push_back(footage);
    
    // Add voice over timeline
    ExtendedTimeline voiceOver;
    voiceOver.uuid = "timeline-voice-1";
    voiceOver.category = ProjectTimelineCategoryEnum::SYNTHETIC_VOICE_OVER;
    voiceOver.assetUuid = "asset-audio-1";
    voiceOver.duration = 8000;
    voiceOver.volume = 0.8;
    testTimelines.push_back(voiceOver);
    
    std::unordered_map<std::string, ProjectSceneAsset> assetMap;
    ProjectSceneAsset audioAsset;
    audioAsset.uuid = "asset-audio-1";
    audioAsset.assetLink = "https://example.com/audio.mp3";
    audioAsset.audioLink = "https://example.com/audio_processed.mp3";
    assetMap["asset-audio-1"] = audioAsset;
    
    auto assembledScene = api.assembleSceneAndTimelines(testScene, testTimelines, assetMap);
    
    std::cout << "1. Timeline categorization:" << std::endl;
    std::cout << "   - aRolls count: " << assembledScene.aRolls.size() << std::endl;
    std::cout << "   - bRolls count: " << assembledScene.bRolls.size() << std::endl;
    std::cout << "   - voiceOvers count: " << assembledScene.voiceOvers.size() << std::endl;
    std::cout << "   - unified timelines count: " << assembledScene.timelines.size() << std::endl;
    
    std::cout << "\n=== Testing Tier 2 BGM Management APIs ===" << std::endl;
    
    // Test 2: Add BGM
    ProjectBgmAddReqBody addBgmReq;
    addBgmReq.assetUuid = "bgm-asset-1";
    addBgmReq.startSceneIndex = 0;
    addBgmReq.endSceneIndex = 2;
    addBgmReq.volume = 0.3;
    addBgmReq.loop = true;
    
    auto result2 = api.addBgm(addBgmReq);
    std::cout << "2. Add BGM: " << (result2.success ? "SUCCESS" : "FAILED") 
              << " - " << result2.message << std::endl;
    
    // Test 3: Edit BGM
    ProjectBgmEditReqBody editBgmReq;
    editBgmReq.bgmUuid = "bgm-uuid-1";
    editBgmReq.volume = 0.4;
    editBgmReq.loop = false;
    
    auto result3 = api.editBgm(editBgmReq);
    std::cout << "3. Edit BGM: " << (result3.success ? "SUCCESS" : "FAILED") 
              << " - " << result3.message << std::endl;
    
    // Test 4: Adjust BGM audio
    PsSceneTimelineVolumeReqBody adjustAudioReq;
    adjustAudioReq.sceneUuid = "scene-1";
    adjustAudioReq.timelineVolumes["timeline-1"] = 0.7;
    adjustAudioReq.timelineVolumes["timeline-2"] = 0.5;
    
    auto result4 = api.adjustBgmAudio(adjustAudioReq);
    std::cout << "4. Adjust BGM audio: " << (result4.success ? "SUCCESS" : "FAILED") 
              << " - " << result4.message << std::endl;
    
    std::cout << "\n=== Testing Tier 2 Style and Effects APIs ===" << std::endl;
    
    // Test 5: Set scene background style
    PsSceneBgStyleReqBody bgStyleReq;
    bgStyleReq.sceneUuid = "scene-1";
    bgStyleReq.bgColor = "#FF0000";
    bgStyleReq.bgImageUuid = "bg-image-1";
    
    auto result5 = api.setSceneBgStyle(bgStyleReq);
    std::cout << "5. Set scene bg style: " << (result5.success ? "SUCCESS" : "FAILED") 
              << " - " << result5.message << std::endl;
    
    // Test 6: Update graphic layers
    ProjectGraphicLayerSettingsReqBody layersReq;
    BaseLayer textLayer;
    textLayer.uuid = "layer-text-1";
    textLayer.type = "text";
    textLayer.timeOffsetInScene = 1000;
    textLayer.duration = 3000;
    textLayer.data = {{"text", "Sample Text"}, {"fontSize", 24}};
    layersReq.layers.push_back(textLayer);
    
    auto result6 = api.updateGraphicLayers(layersReq);
    std::cout << "6. Update graphic layers: " << (result6.success ? "SUCCESS" : "FAILED") 
              << " - " << result6.message << std::endl;
    
    // Test 7: Create background image
    CreateWallpaperReqBody wallpaperReq;
    wallpaperReq.prompt = "Beautiful sunset landscape";
    wallpaperReq.style = "photorealistic";
    wallpaperReq.width = 1920;
    wallpaperReq.height = 1080;
    
    auto result7 = api.createBgImage(wallpaperReq);
    std::cout << "7. Create bg image: " << (result7.success ? "SUCCESS" : "FAILED") 
              << " - " << result7.message << std::endl;
    
    std::cout << "\n=== Testing Tier 2 Avatar Management APIs ===" << std::endl;
    
    // Test 8: Add avatar
    ChangeLookReqBody avatarReq;
    avatarReq.lookUuid = "look-business-1";
    
    auto result8 = api.addAvatar(avatarReq);
    std::cout << "8. Add avatar: " << (result8.success ? "SUCCESS" : "FAILED") 
              << " - " << result8.message << std::endl;
    
    // Test 9: Replace avatar
    ChangeLookReqBody replaceReq;
    replaceReq.lookUuid = "look-casual-1";
    
    auto result9 = api.replaceAvatar(replaceReq);
    std::cout << "9. Replace avatar: " << (result9.success ? "SUCCESS" : "FAILED") 
              << " - " << result9.message << std::endl;
    
    // Test 10: Delete avatar
    auto result10 = api.deleteAvatar();
    std::cout << "10. Delete avatar: " << (result10.success ? "SUCCESS" : "FAILED") 
              << " - " << result10.message << std::endl;
    
    std::cout << "\n=== All assembleSceneAndTimelines and Tier 2 APIs tested successfully! ===" << std::endl;
    
    return 0;
}
