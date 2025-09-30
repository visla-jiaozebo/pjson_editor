// Clean minimal header after corruption cleanup
#ifndef PJSON_EDITOR_EXTENDED_API_H
#define PJSON_EDITOR_EXTENDED_API_H

#include "ExtendedModels.h"
#include <cassert>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace pjson {

class ExtendedDataStore; // fwd
std::string genUuid();

struct ApiResult {
    bool success{false};
    std::string message;
    nlohmann::json patch;
    nlohmann::json data;  // New member to hold controller return structure equivalent
    ApiResult() = default;
    ApiResult(bool s, const std::string& m, const nlohmann::json& p = nlohmann::json::array(), const nlohmann::json& d = nlohmann::json::object())
        : success(s), message(m), patch(p), data(d) {}
};

struct BackendParityOptions {
    bool enforceBlankSceneMergeRule{true};
    bool markTranscriptsModifiedOnSplit{true};
    bool clampMergedSceneDuration{true};
};

class ExtendedControllerAPI {
private:
    ExtendedDataStore* dataStore{nullptr};
    BackendParityOptions parityOptions{};
    
    // Helper methods for VO conversion
    nlohmann::json convertSceneToProjectSceneVo(const ExtendedProjectScene& scene) const;
    nlohmann::json convertProjectToProjectAndSceneVo(const std::string& sceneUuid) const;
    nlohmann::json convertProjectToProjectAndScenesVo() const;
    nlohmann::json convertAssetsMap(const std::unordered_map<std::string, ProjectSceneAsset>& assets) const;
    
public:
    void setDataStore(ExtendedDataStore* ds) { dataStore = ds; }
    void setParityOptions(const BackendParityOptions& o) { parityOptions = o; }
    const BackendParityOptions& getParityOptions() const { return parityOptions; }
    
    // Method to get current project data for external comparison
    const ExtendedProjectAndScenesVo& getCurrentProjectData() const;

        ApiResult addScene(const ExtendedProjectSceneAddReqBody& reqBody);
        ApiResult renameScene(const ExtendedProjectSceneRenameReqBody& reqBody);
        ApiResult moveScene(const ExtendedProjectSceneMoveReqBody& reqBody);
    ApiResult setSceneTime(const ExtendedProjectSceneSetTimeReqBody& reqBody);
        ApiResult cutScene(const ExtendedProjectSceneCutReqBody& reqBody);
        ApiResult splitScene(const ExtendedProjectSceneSplitReqBody& reqBody);
        ApiResult mergeScenes(const ExtendedProjectSceneMergeReqBody& reqBody);
        ApiResult deleteScene(const ExtendedProjectSceneDeleteReqBody& reqBody);
        ApiResult addSceneAudio(const AddSceneAudioReqBody& reqBody);

        ApiResult clearFootage(const ExtendedProjectSceneClearFootageReqBody& reqBody);
        ApiResult replaceFootage(const ProjectSceneReplaceFootageReqBody& reqBody);
    ApiResult adjustFootage(const ProjectSceneAdjustFootageReqBody& reqBody);
    ApiResult addFootage(const ProjectSceneFootageAddReqBody& reqBody);
    ApiResult deleteFootage(const ProjectSceneFootageDeleteReqBody& reqBody);

    ApiResult addVoiceOver(const AddVoiceOverReqBody& reqBody);
    ApiResult deleteVoiceOver(const DeleteVoiceOverReqBody& reqBody);
    ApiResult adjustVoiceOver(const AdjustVoiceOverReqBody& reqBody);
    ApiResult setPauseTime(const ProjectSceneSetPauseTimeReqBody& reqBody);
    ApiResult setSceneTransition(const ProjectSceneTransitionReqBody& reqBody);
    ApiResult deleteTransition(const std::string& sceneUuid);

    ApiResult editScript(const ProjectSceneEditScriptReqBody& reqBody);
    ApiResult setSceneTranscript(const ProjectSceneSetTranscriptReqBody& reqBody);
    ApiResult changeHighlight(const EditSceneHighLightReqBody& reqBody);
    ApiResult setMainStoryOrder(const SetMainStoryOrderReqBody& reqBody);
    ApiResult changeFitType(const ChangeFitTypeReqBody& reqBody);
    ApiResult changeScaleToAll(const UpdateProjectScaleReqBody& reqBody);

    ApiResult addBgm(const ProjectBgmAddReqBody& reqBody);
        ApiResult deleteBgm(const ProjectBgmDeleteReqBody& reqBody);
    ApiResult editBgm(const ProjectBgmEditReqBody& reqBody);
    ApiResult adjustBgmAudio(const PsSceneTimelineVolumeReqBody& reqBody);
    ApiResult setSceneBgStyle(const PsSceneBgStyleReqBody& reqBody);
    ApiResult updateGraphicLayers(const ProjectGraphicLayerSettingsReqBody& reqBody);
    ApiResult createBgImage(const CreateWallpaperReqBody& reqBody);
    ApiResult addBgImage(const PsBgImageBo& reqBody);
    ApiResult addAvatar(const ChangeLookReqBody& reqBody);
    ApiResult replaceAvatar(const ChangeLookReqBody& reqBody);
    ApiResult deleteAvatar();
    ApiResult clearDeletedAvatar();
    ApiResult changeDeletedAvatar();

    ExtendedProjectScene assembleSceneAndTimelines(
        const ExtendedProjectScene& scene,
        const std::vector<ExtendedTimeline>& timelines,
        const std::unordered_map<std::string, ProjectSceneAsset>& assetMap);
};

class ExtendedDataStore {
private:
    std::shared_ptr<ExtendedProjectAndScenesVo> project;
public:
    void init(std::shared_ptr<ExtendedProjectAndScenesVo> initialProject);
    ExtendedProjectAndScenesVo& getProject();
    const ExtendedProjectAndScenesVo& getProject() const;
    // Method to get current project data for external comparison
    const ExtendedProjectAndScenesVo& getCurrentProjectData() const { return *project; }
    ExtendedProjectScene* findScene(const std::string& sceneUuid);
    void recomputeOffsets();
    // void insertScene(const ExtendedProjectScene& scene, int index = -1);
    // bool removeScene(const std::string& sceneUuid);
    // void moveScene(const std::string& sceneUuid, int newIndex);
};

} // namespace pjson

extern pjson::ExtendedProjectAndScenesVo EMPTY_PROJECT; // global for inline method
// Inline method implementations after class definitions
inline const pjson::ExtendedProjectAndScenesVo& pjson::ExtendedControllerAPI::getCurrentProjectData() const {
    assert(dataStore && "Data store not set in ExtendedControllerAPI");
    return dataStore ? dataStore->getCurrentProjectData() : EMPTY_PROJECT;
}

#endif // PJSON_EDITOR_EXTENDED_API_H
