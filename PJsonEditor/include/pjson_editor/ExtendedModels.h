#ifndef PJSON_EDITOR_EXTENDED_MODELS_H
#define PJSON_EDITOR_EXTENDED_MODELS_H

#include <string>
#include <vector>
#include <optional>
#include <map>
#include <unordered_map>
#include <variant>
#include <nlohmann/json.hpp>

namespace pjson {

using json = nlohmann::json;

// Enums matching backend
enum class SceneTypeEnum { DEFAULT, INTRO, OUTRO, BLANK_SCENE };
enum class ProjectTimelineCategoryEnum { 
    MAIN_STORY, INTRO, OUTRO, FOOTAGE, RECORD_VOICE_OVER, 
    SYNTHETIC_VOICE_OVER, STORY_AUDIO, NARRATION_VOICE_OVER,
    AROLL, BROLL, VOICE_OVER  // Additional values for compatibility
};
enum class StatusEnum { ACTIVE, DELETED, DRAFT };
enum class HighlightTypeEnum { KEYWORDS, SENTENCES };
enum class HighlightStyleTypeEnum { WORD_HIGHLIGHT, BACKGROUND_HIGHLIGHT };
enum class EntityTypeEnum { CLIP, PROJECT_ASSET };

// Core timeline with full backend fields
struct ExtendedTimeline {
    std::string uuid;
    std::string sceneUuid;
    std::string projectUuid;
    std::string assetUuid;
    ProjectTimelineCategoryEnum category;
    int timeOffsetInScene{0};    // ms offset within scene
    int timeOffsetInProject{0};  // ms offset within project
    int duration{0};             // ms duration
    int startTime{0};           // asset start time
    int endTime{0};             // asset end time
    double volume{1.0};         // volume multiplier
    bool mute{false};
    double speed{1.0};
    std::string blendMode{"normal"}; // visual blend mode (e.g., normal, multiply)
    
    // Crop and animation data
    std::optional<json> cropData;
    std::optional<json> kenburnsData;
    
    // Internal IDs (for patch generation)
    std::optional<std::string> id;
    std::optional<std::string> assetId;
};

// Voice over specific structure
struct VoiceOver {
    std::string uuid;
    std::string assetUuid;
    std::string sceneUuid;
    std::string projectUuid;
    ProjectTimelineCategoryEnum category;
    int timeOffsetInProject{0};
    int duration{0};
    int startTime{0};
    int endTime{0};
    double volume{1.0};
    std::string audioLink;
    std::string voiceUuid;
    
    // Narration specific fields
    bool audioOnly{false};
    std::optional<std::string> shape;
    std::optional<double> scale;
    std::optional<json> position;
    bool usePosition{false};
};

// Transcript structures
struct TranscriptItem {
    int startMs{0};
    int endMs{0};
    std::string text;
    std::optional<std::string> speaker;
    std::optional<std::vector<std::string>> keywords;
};

struct SceneTranscript {
    std::string text;
    std::vector<TranscriptItem> items;
    std::vector<std::string> originalKeywords;
    bool modified{false};
    int duration{0}; // Duration in milliseconds
    
    // Transcript modification status
    struct ModificationStatus {
        bool changed{false};
        bool voiceRedo{false};
        bool recommendFootageRedo{false};
    } modificationStatus;
};

// Asset structures
struct ProjectSceneAsset {
    std::string assetId;
    std::string uuid;
    std::string assetLink;
    std::optional<std::string> audioLink;
    std::string assetType;
    std::optional<std::string> coverLink;
    int duration{0};
    std::optional<std::string> mediaId;
    bool newMedia{false};
    
    // Voice/audio specific
    std::optional<std::string> voiceId;
    std::optional<json> aiTags;
    
    // Media metadata
    std::optional<int> width;
    std::optional<int> height;
    std::optional<std::string> format;
};

// Layer structures (simplified union approach)
struct BaseLayer {
    std::string uuid;
    std::string type; // "text", "avatar", "sticker", "narration", etc.
    int timeOffsetInScene{0};
    int duration{0};
    json data; // type-specific data as JSON
};

// Transition structure
struct SceneTransition {
    std::string type;
    int duration{0};
    std::optional<json> easing;
    std::optional<json> properties;
};

// Text on screen structure
struct SubtitleText {
    std::string fullText;
    std::vector<std::string> offsetTexts;
    HighlightTypeEnum highlightType{HighlightTypeEnum::KEYWORDS};
    HighlightStyleTypeEnum highlightStyleType{HighlightStyleTypeEnum::WORD_HIGHLIGHT};
};

struct TextOnScreen {
    std::optional<SubtitleText> subtitleText;
    std::optional<json> additionalTextLayers;
};

// Volume configuration
struct SceneVolumeConf {
    double bgmVolume{0.5};
    double voiceVolume{1.0};
    double footageVolume{0.3};
    std::unordered_map<std::string, double> timelineVolumes; // timeline uuid -> volume
};

// Style structures
struct SceneEffect {
    std::string animationType;
    std::optional<json> parameters;
};

struct SceneScale {
    double scaleX{1.0};
    double scaleY{1.0};
    double offsetX{0.0};
    double offsetY{0.0};
    std::optional<json> cropRect;
};

// BGM structure
struct ProjectBgm {
    std::string uuid;
    std::string assetUuid;  // asset_uuid from protobuf
    std::string assetLink;
    std::optional<std::string> adjustedBgmLink;
    int startTime{0};
    int duration{0};
    double volume{0.5};
    bool loop{true};
};

// Synthetic voice metadata
struct SyntheticVoiceMetadata {
    std::string voiceId;
    std::string voiceName;
    std::string language;
    std::string gender;
    std::optional<json> additionalParams;
};

// Enhanced scene structure
struct ExtendedProjectScene {
    std::string uuid;
    std::string projectUuid;
    std::string name;
    SceneTypeEnum sceneType{SceneTypeEnum::DEFAULT};
    int duration{0};
    int timeOffsetInProject{0};
    std::optional<int> pauseTime;
    int audioFlag{0};
    
    // Content
    std::optional<SceneTranscript> transcript;
    
    // Timeline categories (replacing simple timelines vector)
    std::vector<ExtendedTimeline> aRolls;
    std::vector<ExtendedTimeline> bRolls;
    std::vector<VoiceOver> voiceOvers;
    
    // Additional scene elements
    std::vector<BaseLayer> layers;
    std::vector<SceneTransition> transitions;
    std::optional<TextOnScreen> textOnScreen;
    std::optional<SceneVolumeConf> audio;
    std::optional<SceneEffect> effect;
    std::optional<SceneScale> scale;
    
    // Deprecated/compatibility fields
    std::optional<json> deprecatedEffect;
    std::optional<json> deprecatedScale;
    std::optional<json> deprecatedAudio;
    
    // Policy and metadata
    std::optional<int> brollShorterPolicyKey;
    std::optional<double> bgmVolume;
    
    // Legacy compatibility - unified timelines
    std::vector<ExtendedTimeline> timelines; // computed from aRolls + bRolls + voiceOvers
};

// Enhanced project structure
// Project metadata structure
struct ExtendedProject {
    std::string uuid;
    std::string name;
    std::string ownerUuid;
    StatusEnum status{StatusEnum::ACTIVE};
    std::optional<std::string> padColor;
    std::optional<int> videoFormat;
    std::optional<int> brollShorterPolicyKey;
    std::optional<bool> syntheticAll;
    int version{1};
};

struct ExtendedProjectAndScenesVo {
    ExtendedProject project;
    std::string projectUuid;
    std::string ownerUuid;
    StatusEnum status{StatusEnum::ACTIVE};

    public:
        ExtendedProjectAndScenesVo() = default;
        ExtendedProjectAndScenesVo(const nlohmann::json &scenes);
    // Content
    std::vector<ExtendedProjectScene> scenes;
    std::vector<ExtendedTimeline> timelines; // Global timeline list for backward compatibility
    std::unordered_map<std::string, ProjectSceneAsset> assets; // assetId -> asset
    std::vector<ProjectBgm> bgms;
    std::unordered_map<std::string, SyntheticVoiceMetadata> syntheticVoices; // assetId -> voice
    
    // Style (simplified)
    std::optional<json> style;
    std::optional<json> text; // deprecated text style
    
    // Internal tracking
    int version{1}; // for conflict detection
};

// Request body structures (enhanced)
struct ExtendedProjectSceneAddReqBody {
    ExtendedProjectSceneAddReqBody() = default;
    ExtendedProjectSceneAddReqBody(const nlohmann::json &_data){
        addPosition = _data["addPosition"];
        duration = _data.value("duration", 0);
    };
    int addPosition;              // 与API参数保持一致：position(index) to be added
    std::optional<int> duration;  // 与API参数保持一致：scene duration in ms
};

struct ExtendedProjectSceneRenameReqBody { 
    ExtendedProjectSceneRenameReqBody() = default;
    ExtendedProjectSceneRenameReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("name")) name = _data["name"];
    }
    std::string sceneUuid; 
    std::string name; 
};

struct ExtendedProjectSceneMoveReqBody { 
    ExtendedProjectSceneMoveReqBody() = default;
    ExtendedProjectSceneMoveReqBody(const nlohmann::json &_data) {
        if (_data.contains("uuid")) uuid = _data["uuid"];
        if (_data.contains("newIndex")) newIndex = _data["newIndex"];
        if (_data.contains("afterSceneUuid")) afterSceneUuid = _data["afterSceneUuid"];
    }
    std::string uuid; 
    int newIndex;
    std::optional<std::string> afterSceneUuid; // UUID of scene to insert after
};

struct ExtendedProjectSceneSetTimeReqBody { 
    ExtendedProjectSceneSetTimeReqBody() = default;
    ExtendedProjectSceneSetTimeReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("newDuration")) newDuration = _data["newDuration"];
        else if (_data.contains("duration")) newDuration = _data["duration"];
    }
    std::string sceneUuid; 
    int newDuration; 
};

struct TimePeriod {
    int start{0};
    int end{0};
    int getDuration() const { return end - start; }
};

struct ExtendedProjectSceneCutReqBody { 
    ExtendedProjectSceneCutReqBody() = default;
    ExtendedProjectSceneCutReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("cutList") && _data["cutList"].is_array()) {
            for (const auto& period : _data["cutList"]) {
                TimePeriod tp;
                if (period.contains("start")) tp.start = period["start"];
                if (period.contains("end")) tp.end = period["end"];
                cutList.push_back(tp);
            }
        }
    }
    std::string sceneUuid; 
    std::vector<TimePeriod> cutList;
};

struct ExtendedProjectSceneSplitReqBody { 
    ExtendedProjectSceneSplitReqBody() = default;
    ExtendedProjectSceneSplitReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("splitTime")) splitTime = _data["splitTime"];
    }
    std::string sceneUuid; 
    int splitTime; 
};

struct ExtendedProjectSceneMergeReqBody { 
    ExtendedProjectSceneMergeReqBody() = default;
    ExtendedProjectSceneMergeReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuids") && _data["sceneUuids"].is_array()) {
            for (const auto& uuid : _data["sceneUuids"]) {
                sceneUuids.push_back(uuid.get<std::string>());
            }
        }
    }
    std::vector<std::string> sceneUuids; 
};

struct ExtendedProjectSceneDeleteReqBody { 
    ExtendedProjectSceneDeleteReqBody() = default;
    ExtendedProjectSceneDeleteReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
    }
    std::string sceneUuid; 
};

struct ExtendedProjectSceneClearFootageReqBody { 
    ExtendedProjectSceneClearFootageReqBody() = default;
    ExtendedProjectSceneClearFootageReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
    }
    std::string sceneUuid; 
};

// Footage management
struct ProjectSceneReplaceFootageReqBody {
    ProjectSceneReplaceFootageReqBody() = default;
    ProjectSceneReplaceFootageReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("oldTimelineUuid")) oldTimelineUuid = _data["oldTimelineUuid"];
        if (_data.contains("newAssetUuid")) newAssetUuid = _data["newAssetUuid"];
        if (_data.contains("startTime")) startTime = _data["startTime"];
        if (_data.contains("endTime")) endTime = _data["endTime"];
    }
    std::string sceneUuid;
    std::string oldTimelineUuid;
    std::string newAssetUuid;
    std::optional<int> startTime;
    std::optional<int> endTime;
};

struct ProjectSceneAdjustFootageReqBody {
    ProjectSceneAdjustFootageReqBody() = default;
    ProjectSceneAdjustFootageReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("timelineUuid")) timelineUuid = _data["timelineUuid"];
        if (_data.contains("startTime")) startTime = _data["startTime"];
        if (_data.contains("endTime")) endTime = _data["endTime"];
        if (_data.contains("timeOffsetInScene")) timeOffsetInScene = _data["timeOffsetInScene"];
        if (_data.contains("volume")) volume = _data["volume"];
        if (_data.contains("cropData")) cropData = _data["cropData"];
    }
    std::string sceneUuid;
    std::string timelineUuid;
    std::optional<int> startTime;
    std::optional<int> endTime;
    std::optional<int> timeOffsetInScene;
    std::optional<double> volume;
    std::optional<json> cropData;
};

// Voice over management
struct AddVoiceOverReqBody {
    AddVoiceOverReqBody() = default;
    AddVoiceOverReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("assetUuid")) assetUuid = _data["assetUuid"];
        if (_data.contains("timeOffsetInScene")) timeOffsetInScene = _data["timeOffsetInScene"];
        if (_data.contains("duration")) duration = _data["duration"];
        if (_data.contains("audioOnly")) audioOnly = _data["audioOnly"];
        if (_data.contains("shape")) shape = _data["shape"];
    }
    std::string sceneUuid;
    std::string assetUuid;
    int timeOffsetInScene{0};
    int duration{0};
    std::optional<bool> audioOnly;
    std::optional<std::string> shape;
};

// Add Scene Audio request body (matching API)
struct AddSceneAudioReqBody {
    AddSceneAudioReqBody() = default;
    AddSceneAudioReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("entityUuid")) entityUuid = _data["entityUuid"];
        else if (_data.contains("assetUuid")) entityUuid = _data["assetUuid"];
        if (_data.contains("entityType")) {
            std::string typeStr = _data["entityType"];
            if (typeStr == "PROJECT_ASSET") entityType = EntityTypeEnum::PROJECT_ASSET;
            else if (typeStr == "CLIP") entityType = EntityTypeEnum::CLIP;
            else entityType = EntityTypeEnum::PROJECT_ASSET; // default
        } else {
            entityType = EntityTypeEnum::PROJECT_ASSET; // default
        }
    }
    std::string sceneUuid;
    std::string entityUuid;
    EntityTypeEnum entityType;
};

struct ProjectSceneSetPauseTimeReqBody {
    ProjectSceneSetPauseTimeReqBody() = default;
    ProjectSceneSetPauseTimeReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("pauseTime")) pauseTime = _data["pauseTime"];
    }
    std::string sceneUuid;
    int pauseTime{0};
};

struct ProjectSceneTransitionReqBody {
    ProjectSceneTransitionReqBody() = default;
    ProjectSceneTransitionReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("projectUuid")) projectUuid = _data["projectUuid"];
        if (_data.contains("type")) type = _data["type"];
        if (_data.contains("duration")) duration = _data["duration"];
        if (_data.contains("forAllScenes")) forAllScenes = _data["forAllScenes"];
    }
    std::string sceneUuid;
    std::string projectUuid;     // Added to match API
    std::string type;            // Changed from optional<SceneTransition> to direct fields
    int duration{0};
    bool forAllScenes{false};    // Added to match API
};

// Additional Tier 1 request bodies
struct ProjectSceneFootageAddReqBody {
    ProjectSceneFootageAddReqBody() = default;
    ProjectSceneFootageAddReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("assetUuid")) assetUuid = _data["assetUuid"];
        if (_data.contains("timeOffsetInScene")) timeOffsetInScene = _data["timeOffsetInScene"];
        if (_data.contains("duration")) duration = _data["duration"];
        if (_data.contains("startTime")) startTime = _data["startTime"];
        if (_data.contains("endTime")) endTime = _data["endTime"];
        if (_data.contains("cropData")) cropData = _data["cropData"];
    }
    std::string sceneUuid;
    std::string assetUuid;
    int timeOffsetInScene{0};
    int duration{0};
    std::optional<int> startTime;
    std::optional<int> endTime;
    std::optional<json> cropData;
};

struct ProjectSceneFootageDeleteReqBody {
    ProjectSceneFootageDeleteReqBody() = default;
    ProjectSceneFootageDeleteReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("timelineUuid")) timelineUuid = _data["timelineUuid"];
    }
    std::string sceneUuid;
    std::string timelineUuid;
};

struct DeleteVoiceOverReqBody {
    DeleteVoiceOverReqBody() = default;
    DeleteVoiceOverReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("timelineUuid")) timelineUuid = _data["timelineUuid"];
    }
    std::string sceneUuid;
    std::string timelineUuid;
};

struct AdjustVoiceOverReqBody {
    AdjustVoiceOverReqBody() = default;
    AdjustVoiceOverReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("timelineUuid")) timelineUuid = _data["timelineUuid"];
        if (_data.contains("timeOffsetInScene")) timeOffsetInScene = _data["timeOffsetInScene"];
        if (_data.contains("duration")) duration = _data["duration"];
        if (_data.contains("volume")) volume = _data["volume"];
        if (_data.contains("audioOnly")) audioOnly = _data["audioOnly"];
        if (_data.contains("shape")) shape = _data["shape"];
    }
    std::string sceneUuid;
    std::string timelineUuid;
    std::optional<int> timeOffsetInScene;
    std::optional<int> duration;
    std::optional<double> volume;
    std::optional<bool> audioOnly;
    std::optional<std::string> shape;
};

struct ProjectSceneEditScriptReqBody {
    ProjectSceneEditScriptReqBody() = default;
    ProjectSceneEditScriptReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("script")) {
            // Parse SceneTranscript from JSON - simplified version
            auto scriptData = _data["script"];
            if (scriptData.contains("text")) script.text = scriptData["text"];
            if (scriptData.contains("duration")) script.duration = scriptData["duration"];
            if (scriptData.contains("modified")) script.modified = scriptData["modified"];
        }
    }
    std::string sceneUuid;
    SceneTranscript script;
};

struct ProjectSceneSetTranscriptReqBody {
    ProjectSceneSetTranscriptReqBody() = default;
    ProjectSceneSetTranscriptReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("newText")) newText = _data["newText"];
        if (_data.contains("pacePercent")) pacePercent = _data["pacePercent"];
    }
    std::string sceneUuid;
    std::string newText;
    int pacePercent;
};

struct EditSceneHighLightReqBody {
    EditSceneHighLightReqBody() = default;
    EditSceneHighLightReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("highLights") && _data["highLights"].is_array()) {
            for (const auto& highlight : _data["highLights"]) {
                highLights.push_back(highlight.get<std::string>());
            }
        }
    }
    std::string sceneUuid;
    std::vector<std::string> highLights; // highlight keywords
};

struct SetMainStoryOrderReqBody {
    SetMainStoryOrderReqBody() = default;
    SetMainStoryOrderReqBody(const nlohmann::json &_data) {
        if (_data.contains("timelineUuids") && _data["timelineUuids"].is_array()) {
            for (const auto& uuid : _data["timelineUuids"]) {
                timelineUuids.push_back(uuid.get<std::string>());
            }
        }
    }
    std::vector<std::string> timelineUuids;
};

struct ChangeFitTypeReqBody {
    ChangeFitTypeReqBody() = default;
    ChangeFitTypeReqBody(const nlohmann::json &_data) {
        if (_data.contains("fitType")) fitType = _data["fitType"];
    }
    int fitType{0}; // 0=fit, 1=fill, 2=stretch
};

struct UpdateProjectScaleReqBody {
    UpdateProjectScaleReqBody() = default;
    UpdateProjectScaleReqBody(const nlohmann::json &_data) {
        if (_data.contains("timelineUuid")) timelineUuid = _data["timelineUuid"];
        if (_data.contains("scale")) {
            auto scaleData = _data["scale"];
            if (scaleData.contains("scaleX")) scale.scaleX = scaleData["scaleX"];
            if (scaleData.contains("scaleY")) scale.scaleY = scaleData["scaleY"];
            if (scaleData.contains("offsetX")) scale.offsetX = scaleData["offsetX"];
            if (scaleData.contains("offsetY")) scale.offsetY = scaleData["offsetY"];
            if (scaleData.contains("cropRect")) scale.cropRect = scaleData["cropRect"];
        }
    }
    std::string timelineUuid;
    SceneScale scale;
};

// Tier 2 BGM Management request bodies
struct ProjectBgmAddReqBody {
    ProjectBgmAddReqBody() = default;
    ProjectBgmAddReqBody(const nlohmann::json &_data) {
        if (_data.contains("assetUuid")) assetUuid = _data["assetUuid"];
        if (_data.contains("startSceneIndex")) startSceneIndex = _data["startSceneIndex"];
        if (_data.contains("endSceneIndex")) endSceneIndex = _data["endSceneIndex"];
        if (_data.contains("volume")) volume = _data["volume"];
        if (_data.contains("loop")) loop = _data["loop"];
    }
    std::string assetUuid;
    int startSceneIndex{0};
    int endSceneIndex{0};
    double volume{0.5};
    bool loop{true};
};

struct ProjectBgmDeleteReqBody {
    ProjectBgmDeleteReqBody() = default;
    ProjectBgmDeleteReqBody(const nlohmann::json &_data) {
        if (_data.contains("timelineUuid")) timelineUuid = _data["timelineUuid"];
    }
    std::string timelineUuid;  // 修改为与API一致的参数名
};

struct ProjectBgmEditReqBody {
    ProjectBgmEditReqBody() = default;
    ProjectBgmEditReqBody(const nlohmann::json &_data) {
        if (_data.contains("bgmUuid")) bgmUuid = _data["bgmUuid"];
        if (_data.contains("volume")) volume = _data["volume"];
        if (_data.contains("loop")) loop = _data["loop"];
        if (_data.contains("startSceneIndex")) startSceneIndex = _data["startSceneIndex"];
        if (_data.contains("endSceneIndex")) endSceneIndex = _data["endSceneIndex"];
    }
    std::string bgmUuid;
    std::optional<double> volume;
    std::optional<bool> loop;
    std::optional<int> startSceneIndex;
    std::optional<int> endSceneIndex;
};

struct PsSceneTimelineVolumeReqBody {
    PsSceneTimelineVolumeReqBody() = default;
    PsSceneTimelineVolumeReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("timelineVolumes") && _data["timelineVolumes"].is_object()) {
            for (auto& [key, value] : _data["timelineVolumes"].items()) {
                timelineVolumes[key] = value.get<double>();
            }
        }
    }
    std::string sceneUuid;
    std::unordered_map<std::string, double> timelineVolumes; // timelineUuid -> volume
};

// Style and effects request bodies
struct PsSceneBgStyleReqBody {
    PsSceneBgStyleReqBody() = default;
    PsSceneBgStyleReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("bgColor")) bgColor = _data["bgColor"];
        if (_data.contains("bgImageUuid")) bgImageUuid = _data["bgImageUuid"];
    }
    std::string sceneUuid;
    std::string bgColor;
    std::optional<std::string> bgImageUuid;
};

struct ProjectGraphicLayerSettingsReqBody {
    ProjectGraphicLayerSettingsReqBody() = default;
    ProjectGraphicLayerSettingsReqBody(const nlohmann::json &_data) {
        if (_data.contains("layers") && _data["layers"].is_array()) {
            for (const auto& layerData : _data["layers"]) {
                BaseLayer layer;
                if (layerData.contains("uuid")) layer.uuid = layerData["uuid"];
                if (layerData.contains("type")) layer.type = layerData["type"];
                if (layerData.contains("timeOffsetInScene")) layer.timeOffsetInScene = layerData["timeOffsetInScene"];
                if (layerData.contains("duration")) layer.duration = layerData["duration"];
                if (layerData.contains("data")) layer.data = layerData["data"];
                layers.push_back(layer);
            }
        }
    }
    std::vector<BaseLayer> layers;
};

struct CreateWallpaperReqBody {
    CreateWallpaperReqBody() = default;
    CreateWallpaperReqBody(const nlohmann::json &_data) {
        if (_data.contains("prompt")) prompt = _data["prompt"];
        if (_data.contains("style")) style = _data["style"];
        if (_data.contains("width")) width = _data["width"];
        if (_data.contains("height")) height = _data["height"];
    }
    std::string prompt;
    std::string style;
    int width{1920};
    int height{1080};
};

struct PsBgImageBo {
    PsBgImageBo() = default;
    PsBgImageBo(const nlohmann::json &_data) {
        if (_data.contains("imageUrl")) imageUrl = _data["imageUrl"];
        if (_data.contains("name")) name = _data["name"];
        if (_data.contains("description")) description = _data["description"];
    }
    std::string imageUrl;
    std::string name;
    std::optional<std::string> description;
};

// Avatar management request bodies
struct ChangeLookReqBody {
    ChangeLookReqBody() = default;
    ChangeLookReqBody(const nlohmann::json &_data) {
        if (_data.contains("lookUuid")) lookUuid = _data["lookUuid"];
    }
    std::string lookUuid;
};

// Serialization helpers (basic implementations)
NLOHMANN_JSON_SERIALIZE_ENUM(SceneTypeEnum, {
    {SceneTypeEnum::DEFAULT, "default"},
    {SceneTypeEnum::INTRO, "intro"},
    {SceneTypeEnum::OUTRO, "outro"},
    {SceneTypeEnum::BLANK_SCENE, "blankScene"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectTimelineCategoryEnum, {
    {ProjectTimelineCategoryEnum::MAIN_STORY, "MAIN_STORY"},
    {ProjectTimelineCategoryEnum::INTRO, "INTRO"},
    {ProjectTimelineCategoryEnum::OUTRO, "OUTRO"},
    {ProjectTimelineCategoryEnum::FOOTAGE, "FOOTAGE"},
    {ProjectTimelineCategoryEnum::RECORD_VOICE_OVER, "RECORD_VOICE_OVER"},
    {ProjectTimelineCategoryEnum::SYNTHETIC_VOICE_OVER, "SYNTHETIC_VOICE_OVER"},
    {ProjectTimelineCategoryEnum::STORY_AUDIO, "STORY_AUDIO"},
    {ProjectTimelineCategoryEnum::NARRATION_VOICE_OVER, "NARRATION_VOICE_OVER"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(StatusEnum, {
    {StatusEnum::ACTIVE, "ACTIVE"},
    {StatusEnum::DELETED, "DELETED"},
    {StatusEnum::DRAFT, "DRAFT"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(EntityTypeEnum, {
    {EntityTypeEnum::CLIP, "CLIP"},
    {EntityTypeEnum::PROJECT_ASSET, "PROJECT_ASSET"}
})

} // namespace pjson

#endif // PJSON_EDITOR_EXTENDED_MODELS_H
