#ifndef PJSON_EDITOR_EXTENDED_MODELS_H
#define PJSON_EDITOR_EXTENDED_MODELS_H

#include <string>
#include <vector>
#include <optional>
#include <map>
#include <unordered_map>
#include <variant>
#include <cmath>
#include <nlohmann/json.hpp>

namespace pjson {

using json = nlohmann::json;

// Enums matching backend
enum class SceneTypeEnum { DEFAULT, INTRO, OUTRO, BLANK_SCENE };
enum class ProjectTimelineCategoryEnum { 
    MAIN_STORY, INTRO, OUTRO, FOOTAGE, RECORD_VOICE_OVER, 
    SYNTHETIC_VOICE_OVER, STORY_AUDIO, NARRATION_VOICE_OVER,
    AROLL, BROLL, VOICE_OVER,  // Additional values for compatibility
    BACKGROUND_MUSIC  // BGM timeline category
};
enum class StatusEnum { ACTIVE, DELETED, DRAFT };
enum class HighlightTypeEnum { KEYWORDS, SENTENCES };
enum class HighlightStyleTypeEnum { WORD_HIGHLIGHT, BACKGROUND_HIGHLIGHT };

// Text positioning and alignment enums matching Java
enum class TextOnScreenPositionTypeEnum {
    TOP_LEFT = 11,
    TOP_CENTER = 12,
    TOP_RIGHT = 13,
    MIDDLE_LEFT = 21,
    MIDDLE_CENTER = 22,
    MIDDLE_RIGHT = 23,
    BOTTOM_LEFT = 31,
    BOTTOM_CENTER = 32,
    BOTTOM_RIGHT = 33
};

enum class TextAlignEnum {
    NONE = 0,
    LEFT = 1,
    CENTER = 2,
    RIGHT = 3
};

// Subtitle animation type enum matching Java SubtitleAnimationTypeEnum
enum class SubtitleAnimationTypeEnum {
    WORD_HIGHLIGHT = 1,
    BOX_HIGHLIGHT = 2,
    POPING = 3
};

// Transcript modification status enum matching Java TranscriptModifiedStatusEnum
enum class TranscriptModifiedStatusEnum {
    NO_CHANGED,
    CHANGED,
    SYNTHETIC_REDO,
    RECOMMEND_FOOTAGE_REDO,
    ALL_REDO
};

// Media and Asset Type Enums matching Java
enum class MediaTypeEnum { 
    ALL = 0, VIDEO = 1, AUDIO = 2, IMAGE = 3, PDF = 4, PPT = 5 
};

enum class ProjectAssetTypeEnum {
    GROUP_RECORDING = 1,
    SELF_RECORDING = 2,
    IMPORTED_FROM_LIBRARY = 3,
    IMPORTED_FROM_SHARING = 4,
    IMPORTED_FROM_STOCK = 5,
    UPLOADED_FROM_LOCAL = 6,
    COMPOSED_BY_SEQUENCE = 7,
    IMPORTED_FROM_INTEGRATE = 8,
    IMPORTED_FROM_RECOMMENDATION = 9,
    GENERATED_BY_AI = 10,
    QUICK_CAPTURE = 12,
    EXPORT_FROM_PROJECT_VIDEO = 14,
    DOWNLOADED_FROM_EXTERNAL = 15,
    IMPORTED_FROM_PERSONAL_STOCK = 16,
    SCENE_VOICE_OVER = 17,
    SCENES_SYNTHETIC_VOICE_OVER = 18,
    STEP_VIDEO_PICTURES = 19,
    DOCUMENT_SCREEN_SHOT = 20,
    DOCUMENT_INLINE = 21,
    SCENE_NARRATION_VOICE_OVER = 22,
    AVATAR_PREVIEW = 25
};

enum class StockLicenseEnum {
    ROYALTY_FREE,
    RIGHTS_MANAGED,
    EDITORIAL_USE,
    EXTENDED_LICENSE
};

enum class StockProviderCodeEnum {
    PEXELS,
    STORYBLOCKS,
    GETTYIMAGES,
    UNSPLASH,
    PIXABAY
};

enum class ResolutionEnum {
    RES360P = 1
};

enum class EntityTypeEnum { 
    UNKNOWN = -1,
    USER = 0, 
    CLIP = 1, 
    FOLDER = 2, 
    PROJECT = 3, 
    PROJECT_ASSET = 4,
    PROJECT_SEQUENCE = 5, 
    PROJECT_VIDEO = 6, 
    STOCK_MEDIA = 7,
    STOCK_AUDIO = 8, 
    VOICE = 9, 
    PICTURES = 10, 
    VISLA_MEDIA = 11,
    STOCK_MEDIA_PEXELS = 12, 
    STOCK_MEDIA_STORYBLOCKS = 13, 
    STOCK_MEDIA_GETTYIMAGES = 14,
    AGENT_PROJECT = 15, 
    AGENT_PROJECT_MEDIA = 16, 
    CUSTOM_VOICE = 17,
    AVATAR = 18, 
    AVATAR_LOOK = 19,
    WORKSPACE = 50, 
    TEAMSPACE = 51 
};

// Core timeline with full backend fields
struct ExtendedTimeline {
    ExtendedTimeline() = default;
    ExtendedTimeline(const nlohmann::json& timelineJson) {
        if (timelineJson.contains("timelineUuid")) {
            uuid = timelineJson["timelineUuid"].get<std::string>();
        }
        if (timelineJson.contains("sceneUuid")) {
            sceneUuid = timelineJson["sceneUuid"].get<std::string>();
        }
        if (timelineJson.contains("projectUuid")) {
            projectUuid = timelineJson["projectUuid"].get<std::string>();
        }
        if (timelineJson.contains("assetUuid")) {
            assetUuid = timelineJson["assetUuid"].get<std::string>();
        }
        if (timelineJson.contains("category")) {
            // Parse category enum - would need proper mapping
            std::string categoryStr = timelineJson["category"].get<std::string>();
            if (categoryStr == "MAIN_STORY") category = ProjectTimelineCategoryEnum::MAIN_STORY;
            else if (categoryStr == "FOOTAGE") category = ProjectTimelineCategoryEnum::FOOTAGE;
            else if (categoryStr == "VOICE_OVER") category = ProjectTimelineCategoryEnum::VOICE_OVER;
            else if (categoryStr == "BACKGROUND_MUSIC") category = ProjectTimelineCategoryEnum::BACKGROUND_MUSIC;
            else category = ProjectTimelineCategoryEnum::MAIN_STORY; // default
        }
        if (timelineJson.contains("timeOffsetInScene")) {
            timeOffsetInScene = timelineJson["timeOffsetInScene"].get<int>();
        }
        if (timelineJson.contains("timeOffsetInProject")) {
            timeOffsetInProject = timelineJson["timeOffsetInProject"].get<int>();
        }
        if (timelineJson.contains("duration") || timelineJson.contains("timelineDuration")) {
            duration = timelineJson.contains("duration") ? 
                       timelineJson["duration"].get<int>() : 
                       timelineJson["timelineDuration"].get<int>();
        }
        if (timelineJson.contains("startTime")) {
            startTime = timelineJson["startTime"].get<int>();
        }
        if (timelineJson.contains("endTime")) {
            endTime = timelineJson["endTime"].get<int>();
        }
        if (timelineJson.contains("volume")) {
            volume = timelineJson["volume"].get<double>();
        }
        if (timelineJson.contains("mute")) {
            mute = timelineJson["mute"].get<bool>();
        }
        if (timelineJson.contains("speed")) {
            speed = timelineJson["speed"].get<double>();
        }
        if (timelineJson.contains("blendMode")) {
            blendMode = timelineJson["blendMode"].get<std::string>();
        }
        if (timelineJson.contains("cropData")) {
            cropData = timelineJson["cropData"];
        }
        if (timelineJson.contains("kenburnsData")) {
            kenburnsData = timelineJson["kenburnsData"];
        }
        if (timelineJson.contains("id")) {
            id = timelineJson["id"].get<std::string>();
        }
        if (timelineJson.contains("assetId")) {
            assetId = timelineJson["assetId"].get<std::string>();
        }
    }
    
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
    
    // 添加 toJson 方法用于序列化
    json toJson() const {
        json result;
        
        result["timelineUuid"] = uuid;
        result["sceneUuid"] = sceneUuid;
        result["projectUuid"] = projectUuid;
        result["assetUuid"] = assetUuid;
        
        // 分类枚举转换
        switch (category) {
            case ProjectTimelineCategoryEnum::MAIN_STORY: result["category"] = "MAIN_STORY"; break;
            case ProjectTimelineCategoryEnum::INTRO: result["category"] = "INTRO"; break;
            case ProjectTimelineCategoryEnum::OUTRO: result["category"] = "OUTRO"; break;
            case ProjectTimelineCategoryEnum::FOOTAGE: result["category"] = "FOOTAGE"; break;
            case ProjectTimelineCategoryEnum::RECORD_VOICE_OVER: result["category"] = "RECORD_VOICE_OVER"; break;
            case ProjectTimelineCategoryEnum::SYNTHETIC_VOICE_OVER: result["category"] = "SYNTHETIC_VOICE_OVER"; break;
            case ProjectTimelineCategoryEnum::STORY_AUDIO: result["category"] = "STORY_AUDIO"; break;
            case ProjectTimelineCategoryEnum::NARRATION_VOICE_OVER: result["category"] = "NARRATION_VOICE_OVER"; break;
            case ProjectTimelineCategoryEnum::AROLL: result["category"] = "AROLL"; break;
            case ProjectTimelineCategoryEnum::BROLL: result["category"] = "BROLL"; break;
            case ProjectTimelineCategoryEnum::VOICE_OVER: result["category"] = "VOICE_OVER"; break;
            case ProjectTimelineCategoryEnum::BACKGROUND_MUSIC: result["category"] = "BACKGROUND_MUSIC"; break;
        }
        
        result["timeOffsetInScene"] = timeOffsetInScene;
        result["timeOffsetInProject"] = timeOffsetInProject;
        result["duration"] = duration;
        result["startTime"] = startTime;
        result["endTime"] = endTime;
        result["volume"] = volume;
        result["mute"] = mute;
        result["speed"] = speed;
        result["blendMode"] = blendMode;
        
        // Optional 字段
        if (cropData.has_value()) {
            result["cropData"] = cropData.value();
        }
        if (kenburnsData.has_value()) {
            result["kenburnsData"] = kenburnsData.value();
        }
        if (id.has_value()) {
            result["id"] = id.value();
        }
        if (assetId.has_value()) {
            result["assetId"] = assetId.value();
        }
        
        return result;
    }
};

// Voice over specific structure
struct VoiceOver {
    VoiceOver() = default;
    VoiceOver(const nlohmann::json& voiceJson) {
        if (voiceJson.contains("voiceUuid")) {
            uuid = voiceJson["voiceUuid"].get<std::string>();
        }
        if (voiceJson.contains("assetUuid")) {
            assetUuid = voiceJson["assetUuid"].get<std::string>();
        }
        if (voiceJson.contains("sceneUuid")) {
            sceneUuid = voiceJson["sceneUuid"].get<std::string>();
        }
        if (voiceJson.contains("projectUuid")) {
            projectUuid = voiceJson["projectUuid"].get<std::string>();
        }
        if (voiceJson.contains("category")) {
            // Parse category enum
            std::string categoryStr = voiceJson["category"].get<std::string>();
            if (categoryStr == "VOICE_OVER") category = ProjectTimelineCategoryEnum::VOICE_OVER;
            else if (categoryStr == "NARRATION_VOICE_OVER") category = ProjectTimelineCategoryEnum::NARRATION_VOICE_OVER;
            else if (categoryStr == "SYNTHETIC_VOICE_OVER") category = ProjectTimelineCategoryEnum::SYNTHETIC_VOICE_OVER;
            else category = ProjectTimelineCategoryEnum::VOICE_OVER; // default
        } else {
            category = ProjectTimelineCategoryEnum::VOICE_OVER; // default
        }
        if (voiceJson.contains("timeOffsetInProject")) {
            timeOffsetInProject = voiceJson["timeOffsetInProject"].get<int>();
        }
        if (voiceJson.contains("duration") || voiceJson.contains("timelineDuration")) {
            duration = voiceJson.contains("duration") ? 
                      voiceJson["duration"].get<int>() : 
                      voiceJson["timelineDuration"].get<int>();
        }
        if (voiceJson.contains("startTime")) {
            startTime = voiceJson["startTime"].get<int>();
        }
        if (voiceJson.contains("endTime")) {
            endTime = voiceJson["endTime"].get<int>();
        }
        if (voiceJson.contains("volume")) {
            volume = voiceJson["volume"].get<double>();
        }
        if (voiceJson.contains("audioLink")) {
            audioLink = voiceJson["audioLink"].get<std::string>();
        }
        if (voiceJson.contains("voiceUuid")) {
            voiceUuid = voiceJson["voiceUuid"].get<std::string>();
        }
        if (voiceJson.contains("audioOnly")) {
            audioOnly = voiceJson["audioOnly"].get<bool>();
        }
        if (voiceJson.contains("shape")) {
            shape = voiceJson["shape"].get<std::string>();
        }
        if (voiceJson.contains("scale")) {
            scale = voiceJson["scale"].get<double>();
        }
        if (voiceJson.contains("position")) {
            position = voiceJson["position"];
        }
        if (voiceJson.contains("usePosition")) {
            usePosition = voiceJson["usePosition"].get<bool>();
        }
    }
    
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
    
    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["uuid"] = uuid;
        result["assetUuid"] = assetUuid;
        result["sceneUuid"] = sceneUuid;
        result["projectUuid"] = projectUuid;
        
        // 分类枚举转换
        switch (category) {
            case ProjectTimelineCategoryEnum::VOICE_OVER: result["category"] = "VOICE_OVER"; break;
            case ProjectTimelineCategoryEnum::NARRATION_VOICE_OVER: result["category"] = "NARRATION_VOICE_OVER"; break;
            case ProjectTimelineCategoryEnum::SYNTHETIC_VOICE_OVER: result["category"] = "SYNTHETIC_VOICE_OVER"; break;
            case ProjectTimelineCategoryEnum::RECORD_VOICE_OVER: result["category"] = "RECORD_VOICE_OVER"; break;
            default: result["category"] = "VOICE_OVER"; break;
        }
        
        result["timeOffsetInProject"] = timeOffsetInProject;
        result["duration"] = duration;
        result["startTime"] = startTime;
        result["endTime"] = endTime;
        result["volume"] = volume;
        result["audioLink"] = audioLink;
        result["voiceUuid"] = voiceUuid;
        result["audioOnly"] = audioOnly;
        result["usePosition"] = usePosition;
        
        // Optional 字段
        if (shape.has_value()) {
            result["shape"] = shape.value();
        }
        if (scale.has_value()) {
            result["scale"] = scale.value();
        }
        if (position.has_value()) {
            result["position"] = position.value();
        }
        
        return result;
    }
};

// Transcript structures
struct TranscriptItemAlternative {
    std::string content;
    std::string confidence;
    
    TranscriptItemAlternative() = default;
    TranscriptItemAlternative(const std::string& content, const std::string& confidence)
        : content(content), confidence(confidence) {}

    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        result["content"] = content;
        result["confidence"] = confidence;
        return result;
    }
};

struct TranscriptItem {
    TranscriptItem() = default;
    TranscriptItem(const nlohmann::json& item) {
        if (item.contains("content") && !item["content"].is_null()) {
            content = item["content"].get<std::string>();
        }
        if (item.contains("start_time") && !item["start_time"].is_null()) {
            start_time = item["start_time"].get<double>();
        }
        if (item.contains("end_time") && !item["end_time"].is_null()) {
            end_time = item["end_time"].get<double>();
        }
        if (item.contains("type") && !item["type"].is_null()) {
            type = item["type"].get<std::string>();
        }
        if (item.contains("sentence_end") && !item["sentence_end"].is_null()) {
            sentence_end = item["sentence_end"].get<bool>();
        }
        if (item.contains("alternatives") && item["alternatives"].is_array()) {
            for (const auto& alt : item["alternatives"]) {
                if (alt.contains("content") && alt.contains("confidence")) {
                    alternatives.emplace_back(
                        alt["content"].get<std::string>(),
                        alt["confidence"].get<std::string>()
                    );
                }
            }
        }
    }
    
    std::string content;  // item content (shortcut of alternatives[0].content)
    double start_time{0.0};  // start time in seconds (matching Java Double)
    double end_time{0.0};    // end time in seconds (matching Java Double)
    std::string type;        // "pronunciation" or "punctuation" or "silent"
    std::optional<bool> sentence_end;  // sentence end flag (matching Java Boolean)
    std::vector<TranscriptItemAlternative> alternatives;  // content/confidence pairs
    
    // Helper method to get duration (matching Java getDuration())
    double getDuration() const {
        return end_time - start_time;
    }

    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["content"] = content;
        result["start_time"] = start_time;
        result["end_time"] = end_time;
        result["type"] = type;
        
        if (sentence_end.has_value()) {
            result["sentence_end"] = sentence_end.value();
        }
        
        result["alternatives"] = nlohmann::json::array();
        for (const auto& alternative : alternatives) {
            result["alternatives"].push_back(alternative.toJson());
        }
        
        return result;
    }
};

struct SceneTranscript {
    SceneTranscript() = default;
    SceneTranscript(const nlohmann::json& _data) {
        if (_data.contains("text")) text = _data["text"];
        if (_data.contains("items") && _data["items"].is_array()) {
            for (const auto& item : _data["items"]) {
                TranscriptItem ti(item);
                items.push_back(ti);
            }
        }
        if (_data.contains("originalKeywords") && _data["originalKeywords"].is_array()) {
            originalKeywords = _data["originalKeywords"].get<std::vector<std::string>>();
        }
        if (_data.contains("modified")) modified = _data["modified"];
        if (_data.contains("duration")) duration = _data["duration"];
    }
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

    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["text"] = text;
        result["modified"] = modified;
        result["duration"] = duration;
        
        result["items"] = nlohmann::json::array();
        for (const auto& item : items) {
            result["items"].push_back(item.toJson());
        }
        
        result["originalKeywords"] = originalKeywords;
        
        return result;
    }
};

// Provider Info structure matching Java ProviderInfoVo
struct ProviderInfoVo {
    ProviderInfoVo() = default;
    ProviderInfoVo(const nlohmann::json& data) {
        if (data.contains("license") && data["license"].is_number()) {
            license = static_cast<StockLicenseEnum>(data["license"].get<int>());
        }
        if (data.contains("providerCode") && data["providerCode"].is_number()) {
            providerCode = static_cast<StockProviderCodeEnum>(data["providerCode"].get<int>());
        }
        if (data.contains("providerLink")) {
            providerLink = data["providerLink"].get<std::string>();
        }
    }
    
    std::optional<StockLicenseEnum> license;
    std::optional<StockProviderCodeEnum> providerCode;
    std::optional<std::string> providerLink;
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        if (license.has_value()) {
            result["license"] = static_cast<int>(license.value());
        }
        if (providerCode.has_value()) {
            result["providerCode"] = static_cast<int>(providerCode.value());
        }
        if (providerLink.has_value()) {
            result["providerLink"] = providerLink.value();
        }
        return result;
    }
};

// Project Asset Videoshot structure matching Java ProjectAssetVideoshotVo
struct ProjectAssetVideoshotVo {
    ProjectAssetVideoshotVo() = default;
    ProjectAssetVideoshotVo(const nlohmann::json& data) {
        if (data.contains("uuid")) uuid = data["uuid"].get<std::string>();
        if (data.contains("assetUuid")) assetUuid = data["assetUuid"].get<std::string>();
        if (data.contains("row")) row = data["row"].get<int>();
        if (data.contains("column")) column = data["column"].get<int>();
        if (data.contains("width")) width = data["width"].get<int>();
        if (data.contains("height")) height = data["height"].get<int>();
        if (data.contains("interval")) interval = data["interval"].get<int>();
        if (data.contains("images") && data["images"].is_array()) {
            images.clear();
            for (const auto& img : data["images"]) {
                images.push_back(img.get<std::string>());
            }
        }
        if (data.contains("id")) id = data["id"].get<long>();
        if (data.contains("assetId")) assetId = data["assetId"].get<long>();
        if (data.contains("baseImgUrl")) baseImgUrl = data["baseImgUrl"].get<std::string>();
        if (data.contains("maxIndex")) maxIndex = data["maxIndex"].get<int>();
    }
    
    std::optional<std::string> uuid;
    std::optional<std::string> assetUuid;
    std::optional<int> row;
    std::optional<int> column;
    std::optional<int> width;
    std::optional<int> height;
    std::optional<int> interval;
    std::vector<std::string> images;
    
    // JsonIgnore fields
    std::optional<long> id;
    std::optional<long> assetId;
    std::optional<std::string> baseImgUrl;
    std::optional<int> maxIndex;
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        if (uuid.has_value()) result["uuid"] = uuid.value();
        if (assetUuid.has_value()) result["assetUuid"] = assetUuid.value();
        if (row.has_value()) result["row"] = row.value();
        if (column.has_value()) result["column"] = column.value();
        if (width.has_value()) result["width"] = width.value();
        if (height.has_value()) result["height"] = height.value();
        if (interval.has_value()) result["interval"] = interval.value();
        
        result["images"] = nlohmann::json::array();
        for (const auto& image : images) {
            result["images"].push_back(image);
        }
        
        return result;
    }
};

// Asset structures
// Project Scene Asset structure matching Java ProjectSceneAssetVo
struct ProjectSceneAsset {
    ProjectSceneAsset() = default;
    ProjectSceneAsset(const nlohmann::json& assetJson) {
        if (assetJson.contains("assetUuid")) {
            assetUuid = assetJson["assetUuid"].get<std::string>();
        }
        if (assetJson.contains("sourceEntityUuid")) {
            sourceEntityUuid = assetJson["sourceEntityUuid"].get<std::string>();
        }
        if (assetJson.contains("mediaType") && assetJson["mediaType"].is_number()) {
            mediaType = static_cast<MediaTypeEnum>(assetJson["mediaType"].get<int>());
        }
        if (assetJson.contains("assetType") && assetJson["assetType"].is_number()) {
            assetType = static_cast<ProjectAssetTypeEnum>(assetJson["assetType"].get<int>());
        }
        if (assetJson.contains("title")) {
            title = assetJson["title"].get<std::string>();
        }
        if (assetJson.contains("assetLink")) {
            assetLink = assetJson["assetLink"].get<std::string>();
        }
        if (assetJson.contains("playbackLink")) {
            playbackLink = assetJson["playbackLink"].get<std::string>();
        }
        if (assetJson.contains("audioLink")) {
            audioLink = assetJson["audioLink"].get<std::string>();
        }
        if (assetJson.contains("thumbnailLink")) {
            thumbnailLink = assetJson["thumbnailLink"].get<std::string>();
        }
        if (assetJson.contains("duration")) {
            duration = assetJson["duration"].get<int>();
        }
        if (assetJson.contains("width")) {
            width = assetJson["width"].get<int>();
        }
        if (assetJson.contains("height")) {
            height = assetJson["height"].get<int>();
        }
        if (assetJson.contains("previewLink")) {
            previewLink = assetJson["previewLink"].get<std::string>();
        }
        if (assetJson.contains("mattingLink")) {
            mattingLink = assetJson["mattingLink"].get<std::string>();
        }
        if (assetJson.contains("videoshot")) {
            videoshot = ProjectAssetVideoshotVo(assetJson["videoshot"]);
        }
        if (assetJson.contains("providerInfo")) {
            providerInfo = ProviderInfoVo(assetJson["providerInfo"]);
        }
        if (assetJson.contains("audioExist")) {
            audioExist = assetJson["audioExist"].get<bool>();
        }
        if (assetJson.contains("usage")) {
            usage = assetJson["usage"].get<std::string>();
        }
        if (assetJson.contains("premium")) {
            premium = assetJson["premium"].get<bool>();
        }
        if (assetJson.contains("isExpired")) {
            isExpired = assetJson["isExpired"].get<bool>();
        }
        if (assetJson.contains("resolution")) {
            resolution = assetJson["resolution"].get<int>();
        }
        if (assetJson.contains("id")) {
            id = assetJson["id"].get<long>();
        }
        if (assetJson.contains("sourceClipId")) {
            sourceClipId = assetJson["sourceClipId"].get<long>();
        }
    }
    
    // Main fields matching Java ProjectSceneAssetVo
    std::optional<std::string> assetUuid;
    std::optional<std::string> sourceEntityUuid;
    std::optional<MediaTypeEnum> mediaType;
    std::optional<ProjectAssetTypeEnum> assetType;
    std::optional<std::string> title;
    std::optional<std::string> assetLink;
    std::optional<std::string> playbackLink;    // m3u8 link
    std::optional<std::string> audioLink;
    std::optional<std::string> thumbnailLink;
    std::optional<int> duration;
    std::optional<int> width;
    std::optional<int> height;
    std::optional<std::string> previewLink;
    std::optional<std::string> mattingLink;     // matting link
    std::optional<ProjectAssetVideoshotVo> videoshot;
    std::optional<ProviderInfoVo> providerInfo;
    bool audioExist = false;
    std::optional<std::string> usage;           // creative/editorial
    bool premium = false;
    bool isExpired = false;
    
    // JsonIgnore fields (private in Java but exposed for C++ convenience)
    std::optional<int> resolution;
    std::optional<long> id;
    std::optional<long> sourceClipId;
    
    // toJson method for serialization
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        if (assetUuid.has_value()) {
            result["assetUuid"] = assetUuid.value();
        }
        if (sourceEntityUuid.has_value()) {
            result["sourceEntityUuid"] = sourceEntityUuid.value();
        }
        if (mediaType.has_value()) {
            result["mediaType"] = static_cast<int>(mediaType.value());
        }
        if (assetType.has_value()) {
            result["assetType"] = static_cast<int>(assetType.value());
        }
        if (title.has_value()) {
            result["title"] = title.value();
        }
        if (assetLink.has_value()) {
            result["assetLink"] = assetLink.value();
        }
        if (playbackLink.has_value()) {
            result["playbackLink"] = playbackLink.value();
        }
        if (audioLink.has_value()) {
            result["audioLink"] = audioLink.value();
        }
        if (thumbnailLink.has_value()) {
            result["thumbnailLink"] = thumbnailLink.value();
        }
        if (duration.has_value()) {
            result["duration"] = duration.value();
        }
        if (width.has_value()) {
            result["width"] = width.value();
        }
        if (height.has_value()) {
            result["height"] = height.value();
        }
        if (previewLink.has_value()) {
            result["previewLink"] = previewLink.value();
        }
        if (mattingLink.has_value()) {
            result["mattingLink"] = mattingLink.value();
        }
        if (videoshot.has_value()) {
            result["videoshot"] = videoshot.value().toJson();
        }
        if (providerInfo.has_value()) {
            result["providerInfo"] = providerInfo.value().toJson();
        }
        
        result["audioExist"] = audioExist;
        if (usage.has_value()) {
            result["usage"] = usage.value();
        }
        result["premium"] = premium;
        result["isExpired"] = isExpired;
        
        // Note: resolution, id, and sourceClipId are JsonIgnore in Java
        // but can be included here for debugging/internal use if needed
        
        return result;
    }
};

// Layer structures (simplified union approach)
struct BaseLayer {
    std::string uuid;
    std::string type; // "text", "avatar", "sticker", "narration", etc.
    int timeOffsetInScene{0};
    int duration{0};
    json data; // type-specific data as JSON
    
    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["uuid"] = uuid;
        result["type"] = type;
        result["timeOffsetInScene"] = timeOffsetInScene;
        result["duration"] = duration;
        result["data"] = data;
        
        return result;
    }
};

// Transition structure
struct SceneTransition {
    SceneTransition() = default;
    SceneTransition(const nlohmann::json& transitionJson) {
        if (transitionJson.contains("type")) {
            type = transitionJson["type"].get<std::string>();
        }
        if (transitionJson.contains("duration")) {
            duration = transitionJson["duration"].get<int>();
        }
        if (transitionJson.contains("easing")) {
            easing = transitionJson["easing"];
        }
        if (transitionJson.contains("properties")) {
            properties = transitionJson["properties"];
        }
    }
    
    std::string type;
    int duration{0};
    std::optional<json> easing;
    std::optional<json> properties;
    
    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["type"] = type;
        result["duration"] = duration;
        
        // Optional 字段
        if (easing.has_value()) {
            result["easing"] = easing.value();
        }
        if (properties.has_value()) {
            result["properties"] = properties.value();
        }
        
        return result;
    }
};

// Text on screen structure

// Supporting structures matching Java CssStyleVo nested classes
struct OutlineVo {
    bool enable{false};
    std::string color;
    float width{40.0f};
    
    OutlineVo() = default;
    OutlineVo(const nlohmann::json& data) {
        if (data.contains("enable") && !data["enable"].is_null()) {
            enable = data["enable"];
        }
        if (data.contains("color") && !data["color"].is_null()) {
            color = data["color"];
        }
        if (data.contains("width") && !data["width"].is_null()) {
            width = data["width"];
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        result["enable"] = enable;
        if (!color.empty()) result["color"] = color;
        result["width"] = width;
        return result;
    }
};

struct CssShadowVo {
    bool enable{false};
    std::string color;
    int style{0};
    int distance{4};
    int angle{90};
    float blur{10.0f};
    
    CssShadowVo() = default;
    CssShadowVo(const nlohmann::json& data) {
        if (data.contains("enable") && !data["enable"].is_null()) {
            enable = data["enable"];
        }
        if (data.contains("color") && !data["color"].is_null()) {
            color = data["color"];
        }
        if (data.contains("style") && !data["style"].is_null()) {
            style = data["style"];
        }
        if (data.contains("distance") && !data["distance"].is_null()) {
            distance = data["distance"];
        }
        if (data.contains("angle") && !data["angle"].is_null()) {
            angle = data["angle"];
        }
        if (data.contains("blur") && !data["blur"].is_null()) {
            blur = data["blur"];
        }
    }
    
    // Calculate X offset (positive right, negative left)
    float getOffsetX() const {
        return distance * std::cos(angle * M_PI / 180.0f);
    }
    
    // Calculate Y offset (positive down, negative up)
    float getOffsetY() const {
        return distance * std::sin(angle * M_PI / 180.0f);
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        result["enable"] = enable;
        if (!color.empty()) result["color"] = color;
        result["style"] = style;
        result["distance"] = distance;
        result["angle"] = angle;
        result["blur"] = blur;
        return result;
    }
};

struct CssBackgroundVo {
    bool enable{false};
    std::string color;
    int radius{16};
    int paddingX{8};
    int paddingY{8};
    int style{0};
    
    CssBackgroundVo() = default;
    CssBackgroundVo(const nlohmann::json& data) {
        if (data.contains("enable") && !data["enable"].is_null()) {
            enable = data["enable"];
        }
        if (data.contains("color") && !data["color"].is_null()) {
            color = data["color"];
        }
        if (data.contains("radius") && !data["radius"].is_null()) {
            radius = data["radius"];
        }
        if (data.contains("paddingX") && !data["paddingX"].is_null()) {
            paddingX = data["paddingX"];
        }
        if (data.contains("paddingY") && !data["paddingY"].is_null()) {
            paddingY = data["paddingY"];
        }
        if (data.contains("style") && !data["style"].is_null()) {
            style = data["style"];
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        result["enable"] = enable;
        if (!color.empty()) result["color"] = color;
        result["radius"] = radius;
        result["paddingX"] = paddingX;
        result["paddingY"] = paddingY;
        result["style"] = style;
        return result;
    }
};

struct HighLightVo {
    int start;
    int end;
    std::string color;
    int fontSize;
    
    HighLightVo() = default;
    HighLightVo(const nlohmann::json& data) {
        if (data.contains("start") && !data["start"].is_null()) {
            start = data["start"];
        }
        if (data.contains("end") && !data["end"].is_null()) {
            end = data["end"];
        }
        if (data.contains("color") && !data["color"].is_null()) {
            color = data["color"];
        }
        if (data.contains("fontSize") && !data["fontSize"].is_null()) {
            fontSize = data["fontSize"];
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        result["start"] = start;
        result["end"] = end;
        if (!color.empty()) result["color"] = color;
        result["fontSize"] = fontSize;
        return result;
    }
};

// CSS Style structure matching Java CssStyleVo
struct CssStyleVo {
    std::string fontFamily;
    int fontSize;
    std::string color;
    std::string fontStyle{"normal"};
    std::string fontWeight{"normal"};
    float lineHeight{1.2f};
    float lineSpace{0.0f};
    int width;
    int maxWidth{1200};
    std::string alignment;
    
    CssShadowVo shadow;
    OutlineVo outline;
    CssBackgroundVo background;
    std::vector<HighLightVo> highLight;
    std::string fontLink;
    
    CssStyleVo() = default;
    CssStyleVo(const nlohmann::json& data) {
        if (data.contains("fontFamily") && !data["fontFamily"].is_null()) {
            fontFamily = data["fontFamily"];
        }
        if (data.contains("fontSize") && !data["fontSize"].is_null()) {
            fontSize = data["fontSize"];
        }
        if (data.contains("color") && !data["color"].is_null()) {
            color = data["color"];
        }
        if (data.contains("fontStyle") && !data["fontStyle"].is_null()) {
            fontStyle = data["fontStyle"];
        }
        if (data.contains("fontWeight") && !data["fontWeight"].is_null()) {
            fontWeight = data["fontWeight"];
        }
        if (data.contains("lineHeight") && !data["lineHeight"].is_null()) {
            lineHeight = data["lineHeight"];
        }
        if (data.contains("lineSpace") && !data["lineSpace"].is_null()) {
            lineSpace = data["lineSpace"];
        }
        if (data.contains("width") && !data["width"].is_null()) {
            width = data["width"];
        }
        if (data.contains("maxWidth") && !data["maxWidth"].is_null()) {
            maxWidth = data["maxWidth"];
        }
        if (data.contains("alignment") && !data["alignment"].is_null()) {
            alignment = data["alignment"];
        }
        if (data.contains("shadow") && !data["shadow"].is_null()) {
            shadow = CssShadowVo(data["shadow"]);
        }
        if (data.contains("outline") && !data["outline"].is_null()) {
            outline = OutlineVo(data["outline"]);
        }
        if (data.contains("background") && !data["background"].is_null()) {
            background = CssBackgroundVo(data["background"]);
        }
        if (data.contains("highLight") && data["highLight"].is_array()) {
            for (const auto& item : data["highLight"]) {
                highLight.emplace_back(HighLightVo(item));
            }
        }
        if (data.contains("fontLink") && !data["fontLink"].is_null()) {
            fontLink = data["fontLink"];
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        if (!fontFamily.empty()) result["fontFamily"] = fontFamily;
        result["fontSize"] = fontSize;
        if (!color.empty()) result["color"] = color;
        result["fontStyle"] = fontStyle;
        result["fontWeight"] = fontWeight;
        result["lineHeight"] = lineHeight;
        result["lineSpace"] = lineSpace;
        result["width"] = width;
        result["maxWidth"] = maxWidth;
        if (!alignment.empty()) result["alignment"] = alignment;
        result["shadow"] = shadow.toJson();
        result["outline"] = outline.toJson();
        result["background"] = background.toJson();
        if (!highLight.empty()) {
            nlohmann::json highlights = nlohmann::json::array();
            for (const auto& h : highLight) {
                highlights.push_back(h.toJson());
            }
            result["highLight"] = highlights;
        }
        if (!fontLink.empty()) result["fontLink"] = fontLink;
        return result;
    }
};

// Animation attributes structure matching Java AnimationAttributes
struct AnimationAttributes {
    std::string color;
    
    AnimationAttributes() = default;
    AnimationAttributes(const nlohmann::json& data) {
        if (data.contains("color") && !data["color"].is_null()) {
            color = data["color"];
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        if (!color.empty()) result["color"] = color;
        return result;
    }
};

// CSS Style Highlight structures matching Java CssStyleHighlightVo
struct CssStyleHighlightVo {
    // Nested class for outline styling
    struct OutlineVo {
        std::string color;
        float width{40.0f};
        
        OutlineVo() = default;
        OutlineVo(const nlohmann::json& data) {
            if (data.contains("color") && !data["color"].is_null()) {
                color = data["color"];
            }
            if (data.contains("width") && !data["width"].is_null()) {
                width = data["width"];
            }
        }
        
        nlohmann::json toJson() const {
            nlohmann::json result;
            if (!color.empty()) result["color"] = color;
            result["width"] = width;
            return result;
        }
    };
    
    // Nested class for shadow styling
    struct CssShadowVo {
        std::string color;
        int style{0};
        int offsetX{5};
        int offsetY{5};
        float blur{5.0f};
        
        CssShadowVo() = default;
        CssShadowVo(const nlohmann::json& data) {
            if (data.contains("color") && !data["color"].is_null()) {
                color = data["color"];
            }
            if (data.contains("style") && !data["style"].is_null()) {
                style = data["style"];
            }
            if (data.contains("offsetX") && !data["offsetX"].is_null()) {
                offsetX = data["offsetX"];
            }
            if (data.contains("offsetY") && !data["offsetY"].is_null()) {
                offsetY = data["offsetY"];
            }
            if (data.contains("blur") && !data["blur"].is_null()) {
                blur = data["blur"];
            }
        }
        
        nlohmann::json toJson() const {
            nlohmann::json result;
            if (!color.empty()) result["color"] = color;
            result["style"] = style;
            result["offsetX"] = offsetX;
            result["offsetY"] = offsetY;
            result["blur"] = blur;
            return result;
        }
    };
    
    // Nested class for background styling
    struct CssBackgroundVo {
        std::string color;
        int radius{16};
        int paddingX{8};
        int paddingY{8};
        
        CssBackgroundVo() = default;
        CssBackgroundVo(const nlohmann::json& data) {
            if (data.contains("color") && !data["color"].is_null()) {
                color = data["color"];
            }
            if (data.contains("radius") && !data["radius"].is_null()) {
                radius = data["radius"];
            }
            if (data.contains("paddingX") && !data["paddingX"].is_null()) {
                paddingX = data["paddingX"];
            }
            if (data.contains("paddingY") && !data["paddingY"].is_null()) {
                paddingY = data["paddingY"];
            }
        }
        
        nlohmann::json toJson() const {
            nlohmann::json result;
            if (!color.empty()) result["color"] = color;
            result["radius"] = radius;
            result["paddingX"] = paddingX;
            result["paddingY"] = paddingY;
            return result;
        }
    };
    
    // Nested class for highlight styling
    struct HighLightVo {
        std::string color;
        int fontSize;
        
        HighLightVo() = default;
        HighLightVo(const nlohmann::json& data) {
            if (data.contains("color") && !data["color"].is_null()) {
                color = data["color"];
            }
            if (data.contains("fontSize") && !data["fontSize"].is_null()) {
                fontSize = data["fontSize"];
            }
        }
        
        nlohmann::json toJson() const {
            nlohmann::json result;
            if (!color.empty()) result["color"] = color;
            result["fontSize"] = fontSize;
            return result;
        }
    };
    
    // Main fields matching Java CssStyleHighlightVo
    CssBackgroundVo box;
    HighLightVo word;
    std::vector<json> words; // List<HighlightItem> words from Java
    
    CssStyleHighlightVo() = default;
    CssStyleHighlightVo(const nlohmann::json& data) {
        if (data.contains("box") && !data["box"].is_null()) {
            box = CssBackgroundVo(data["box"]);
        }
        if (data.contains("word") && !data["word"].is_null()) {
            word = HighLightVo(data["word"]);
        }
        if (data.contains("words") && data["words"].is_array()) {
            for (const auto& item : data["words"]) {
                words.push_back(item);
            }
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        result["box"] = box.toJson();
        result["word"] = word.toJson();
        if (!words.empty()) {
            nlohmann::json wordsArray = nlohmann::json::array();
            for (const auto& w : words) {
                wordsArray.push_back(w);
            }
            result["words"] = wordsArray;
        }
        return result;
    }
};

// Time-based text on screen style structure matching Java PsTimeTextOnScreenStyleBo
struct PsTimeTextOnScreenStyleBo {
    std::string text;
    int start;  // 相对于scene的text的显示开始时间
    int end;    // 相对于scene的text的显示结束时间
    
    PsTimeTextOnScreenStyleBo() = default;
    PsTimeTextOnScreenStyleBo(const nlohmann::json& data) {
        if (data.contains("text") && !data["text"].is_null()) {
            text = data["text"];
        }
        if (data.contains("start") && !data["start"].is_null()) {
            start = data["start"];
        }
        if (data.contains("end") && !data["end"].is_null()) {
            end = data["end"];
        }
        
        // Support legacy field names for backward compatibility
        if (data.contains("startTime") && !data.contains("start")) {
            start = data["startTime"];
        }
        if (data.contains("endTime") && !data.contains("end")) {
            end = data["endTime"];
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        if (!text.empty()) result["text"] = text;
        result["start"] = start;
        result["end"] = end;
        return result;
    }
};

// Legacy alias for backward compatibility
using OffsetText = PsTimeTextOnScreenStyleBo;

// Base text on screen attributes structure matching Java PsTextOnScreenAttributesVo
struct PsTextOnScreenAttributesVo {
    int styleNo;
    std::string color;
    int size;
    std::string font;
    TextOnScreenPositionTypeEnum positionType{TextOnScreenPositionTypeEnum::BOTTOM_CENTER};
    int zIndex{0};
    TextAlignEnum align{TextAlignEnum::CENTER};
    std::string position;
    int width;
    CssStyleVo style;
    std::string text;
    int duration;
    int timeOffsetInScene;
    
    // Positioning and layout
    int positionOffsetX;
    int positionOffsetY;
    std::string backgroundColor;
    int backgroundRadius;
    int backgroundPaddingX;
    int backgroundPaddingY;
    int backgroundStyle;
    
    // Outline settings
    bool outlineEnable{false};
    std::string outlineColor;
    float outlineWidth{40.0f};
    
    // Shadow settings
    bool shadowEnable{false};
    std::string shadowColor;
    int shadowStyle{0};
    int shadowDistance{4};
    int shadowAngle{90};
    float shadowBlur{10.0f};
    
    // Advanced text settings
    std::string fontStyle{"normal"};
    std::string fontWeight{"normal"};
    float lineHeight{1.2f};
    float lineSpace{0.0f};
    int maxWidth{1200};
    std::string alignment;
    std::string fontLink;
    
    PsTextOnScreenAttributesVo() = default;
    PsTextOnScreenAttributesVo(const nlohmann::json& data) {
        if (data.contains("styleNo") && !data["styleNo"].is_null()) {
            styleNo = data["styleNo"];
        }
        if (data.contains("color") && !data["color"].is_null()) {
            color = data["color"];
        }
        if (data.contains("size") && !data["size"].is_null()) {
            size = data["size"];
        }
        if (data.contains("font") && !data["font"].is_null()) {
            font = data["font"];
        }
        if (data.contains("positionType") && !data["positionType"].is_null()) {
            positionType = static_cast<TextOnScreenPositionTypeEnum>(data["positionType"]);
        }
        if (data.contains("zIndex") && !data["zIndex"].is_null()) {
            zIndex = data["zIndex"];
        }
        if (data.contains("align") && !data["align"].is_null()) {
            align = static_cast<TextAlignEnum>(data["align"]);
        }
        if (data.contains("position") && !data["position"].is_null()) {
            position = data["position"];
        }
        if (data.contains("width") && !data["width"].is_null()) {
            width = data["width"];
        }
        if (data.contains("style") && !data["style"].is_null()) {
            style = CssStyleVo(data["style"]);
        }
        if (data.contains("text") && !data["text"].is_null()) {
            text = data["text"];
        }
        if (data.contains("duration") && !data["duration"].is_null()) {
            duration = data["duration"];
        }
        if (data.contains("timeOffsetInScene") && !data["timeOffsetInScene"].is_null()) {
            timeOffsetInScene = data["timeOffsetInScene"];
        }
        if (data.contains("positionOffsetX") && !data["positionOffsetX"].is_null()) {
            positionOffsetX = data["positionOffsetX"];
        }
        if (data.contains("positionOffsetY") && !data["positionOffsetY"].is_null()) {
            positionOffsetY = data["positionOffsetY"];
        }
        if (data.contains("backgroundColor") && !data["backgroundColor"].is_null()) {
            backgroundColor = data["backgroundColor"];
        }
        if (data.contains("backgroundRadius") && !data["backgroundRadius"].is_null()) {
            backgroundRadius = data["backgroundRadius"];
        }
        if (data.contains("backgroundPaddingX") && !data["backgroundPaddingX"].is_null()) {
            backgroundPaddingX = data["backgroundPaddingX"];
        }
        if (data.contains("backgroundPaddingY") && !data["backgroundPaddingY"].is_null()) {
            backgroundPaddingY = data["backgroundPaddingY"];
        }
        if (data.contains("backgroundStyle") && !data["backgroundStyle"].is_null()) {
            backgroundStyle = data["backgroundStyle"];
        }
        if (data.contains("outlineEnable") && !data["outlineEnable"].is_null()) {
            outlineEnable = data["outlineEnable"];
        }
        if (data.contains("outlineColor") && !data["outlineColor"].is_null()) {
            outlineColor = data["outlineColor"];
        }
        if (data.contains("outlineWidth") && !data["outlineWidth"].is_null()) {
            outlineWidth = data["outlineWidth"];
        }
        if (data.contains("shadowEnable") && !data["shadowEnable"].is_null()) {
            shadowEnable = data["shadowEnable"];
        }
        if (data.contains("shadowColor") && !data["shadowColor"].is_null()) {
            shadowColor = data["shadowColor"];
        }
        if (data.contains("shadowStyle") && !data["shadowStyle"].is_null()) {
            shadowStyle = data["shadowStyle"];
        }
        if (data.contains("shadowDistance") && !data["shadowDistance"].is_null()) {
            shadowDistance = data["shadowDistance"];
        }
        if (data.contains("shadowAngle") && !data["shadowAngle"].is_null()) {
            shadowAngle = data["shadowAngle"];
        }
        if (data.contains("shadowBlur") && !data["shadowBlur"].is_null()) {
            shadowBlur = data["shadowBlur"];
        }
        if (data.contains("fontStyle") && !data["fontStyle"].is_null()) {
            fontStyle = data["fontStyle"];
        }
        if (data.contains("fontWeight") && !data["fontWeight"].is_null()) {
            fontWeight = data["fontWeight"];
        }
        if (data.contains("lineHeight") && !data["lineHeight"].is_null()) {
            lineHeight = data["lineHeight"];
        }
        if (data.contains("lineSpace") && !data["lineSpace"].is_null()) {
            lineSpace = data["lineSpace"];
        }
        if (data.contains("maxWidth") && !data["maxWidth"].is_null()) {
            maxWidth = data["maxWidth"];
        }
        if (data.contains("alignment") && !data["alignment"].is_null()) {
            alignment = data["alignment"];
        }
        if (data.contains("fontLink") && !data["fontLink"].is_null()) {
            fontLink = data["fontLink"];
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        result["styleNo"] = styleNo;
        if (!color.empty()) result["color"] = color;
        result["size"] = size;
        if (!font.empty()) result["font"] = font;
        result["positionType"] = static_cast<int>(positionType);
        result["zIndex"] = zIndex;
        result["align"] = static_cast<int>(align);
        if (!position.empty()) result["position"] = position;
        result["width"] = width;
        result["style"] = style.toJson();
        if (!text.empty()) result["text"] = text;
        result["duration"] = duration;
        result["timeOffsetInScene"] = timeOffsetInScene;
        result["positionOffsetX"] = positionOffsetX;
        result["positionOffsetY"] = positionOffsetY;
        if (!backgroundColor.empty()) result["backgroundColor"] = backgroundColor;
        result["backgroundRadius"] = backgroundRadius;
        result["backgroundPaddingX"] = backgroundPaddingX;
        result["backgroundPaddingY"] = backgroundPaddingY;
        result["backgroundStyle"] = backgroundStyle;
        result["outlineEnable"] = outlineEnable;
        if (!outlineColor.empty()) result["outlineColor"] = outlineColor;
        result["outlineWidth"] = outlineWidth;
        result["shadowEnable"] = shadowEnable;
        if (!shadowColor.empty()) result["shadowColor"] = shadowColor;
        result["shadowStyle"] = shadowStyle;
        result["shadowDistance"] = shadowDistance;
        result["shadowAngle"] = shadowAngle;
        result["shadowBlur"] = shadowBlur;
        result["fontStyle"] = fontStyle;
        result["fontWeight"] = fontWeight;
        result["lineHeight"] = lineHeight;
        result["lineSpace"] = lineSpace;
        result["maxWidth"] = maxWidth;
        if (!alignment.empty()) result["alignment"] = alignment;
        if (!fontLink.empty()) result["fontLink"] = fontLink;
        return result;
    }
};

// Subtitle-specific attributes extending the base text attributes matching Java PsSubtitleOnScreenAttributesVo
struct PsSubtitleOnScreenAttributesVo : public PsTextOnScreenAttributesVo {
    // Highlight settings
    bool highlightEnable{false};
    std::string highlightColor;
    
    // Deprecated highlight fields (for compatibility)
    HighlightTypeEnum highlightType{HighlightTypeEnum::KEYWORDS};
    HighlightStyleTypeEnum highlightStyleType{HighlightStyleTypeEnum::WORD_HIGHLIGHT};
    
    // New highlight fields
    int highlightSize{0};
    CssStyleHighlightVo highlightCssStyle;
    
    // Animation settings
    bool animationEnable{false};
    SubtitleAnimationTypeEnum animationType{SubtitleAnimationTypeEnum::WORD_HIGHLIGHT};
    AnimationAttributes animationAttributes;
    
    // Offset texts for timing (using PsTimeTextOnScreenStyleBo)
    std::vector<PsTimeTextOnScreenStyleBo> offsetTexts;
    
    PsSubtitleOnScreenAttributesVo() = default;
    PsSubtitleOnScreenAttributesVo(const nlohmann::json& data) : PsTextOnScreenAttributesVo(data) {
        if (data.contains("highlightEnable") && !data["highlightEnable"].is_null()) {
            highlightEnable = data["highlightEnable"];
        }
        if (data.contains("highlightColor") && !data["highlightColor"].is_null()) {
            highlightColor = data["highlightColor"];
        }
        
        // Deprecated highlight fields
        if (data.contains("highlightType") && !data["highlightType"].is_null()) {
            highlightType = static_cast<HighlightTypeEnum>(data["highlightType"]);
        }
        if (data.contains("highlightStyleType") && !data["highlightStyleType"].is_null()) {
            highlightStyleType = static_cast<HighlightStyleTypeEnum>(data["highlightStyleType"]);
        }
        
        // New highlight fields
        if (data.contains("highlightSize") && !data["highlightSize"].is_null()) {
            highlightSize = data["highlightSize"];
        }
        if (data.contains("highlightCssStyle") && !data["highlightCssStyle"].is_null()) {
            highlightCssStyle = CssStyleHighlightVo(data["highlightCssStyle"]);
        }
        
        if (data.contains("animationEnable") && !data["animationEnable"].is_null()) {
            animationEnable = data["animationEnable"];
        }
        if (data.contains("animationType") && !data["animationType"].is_null()) {
            animationType = static_cast<SubtitleAnimationTypeEnum>(data["animationType"]);
        }
        if (data.contains("animationAttributes") && !data["animationAttributes"].is_null()) {
            animationAttributes = AnimationAttributes(data["animationAttributes"]);
        }
        if (data.contains("offsetTexts") && data["offsetTexts"].is_array()) {
            for (const auto& item : data["offsetTexts"]) {
                offsetTexts.emplace_back(PsTimeTextOnScreenStyleBo(item));
            }
        }
    }
    
    // Java equivalent methods
    bool getHighLightEnable() const {
        return highlightEnable;
    }
    
    bool getHighlightEnable() const {
        return highlightEnable;
    }
    
    CssStyleHighlightVo getHighlightCssStyle() {
        if (highlightCssStyle.box.color.empty() && highlightCssStyle.word.color.empty()) {
            // Initialize with default values like Java version
            highlightCssStyle.box.color = highlightColor;
            highlightCssStyle.word.color = highlightColor;
            highlightCssStyle.word.fontSize = size;
        }
        return highlightCssStyle;
    }
    
    int getTextSize() const {
        return size;
    }
    
    std::string getTextColor() const {
        return color;
    }
    
    std::string getTextFont() const {
        return font;
    }
    
    std::string getTextAlign() const {
        return static_cast<int>(align) == static_cast<int>(TextAlignEnum::LEFT) ? "left" :
               static_cast<int>(align) == static_cast<int>(TextAlignEnum::CENTER) ? "center" :
               static_cast<int>(align) == static_cast<int>(TextAlignEnum::RIGHT) ? "right" : "none";
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result = PsTextOnScreenAttributesVo::toJson(); // Call base class toJson
        
        result["highlightEnable"] = highlightEnable;
        if (!highlightColor.empty()) result["highlightColor"] = highlightColor;
        
        // Deprecated fields (still included for compatibility)
        result["highlightType"] = static_cast<int>(highlightType);
        result["highlightStyleType"] = static_cast<int>(highlightStyleType);
        
        // New highlight fields
        result["highlightSize"] = highlightSize;
        result["highlightCssStyle"] = highlightCssStyle.toJson();
        
        result["animationEnable"] = animationEnable;
        result["animationType"] = static_cast<int>(animationType);
        result["animationAttributes"] = animationAttributes.toJson();
        
        if (!offsetTexts.empty()) {
            nlohmann::json offsetsArray = nlohmann::json::array();
            for (const auto& offset : offsetTexts) {
                offsetsArray.push_back(offset.toJson());
            }
            result["offsetTexts"] = offsetsArray;
        }
        
        return result;
    }
};

// TextOnScreen structure matching Java PsTextOnScreenV2Vo
struct TextOnScreen {
    PsSubtitleOnScreenAttributesVo subtitleText;
    
    TextOnScreen() = default;
    TextOnScreen(const nlohmann::json& data) {
        if (data.contains("subtitleText") && !data["subtitleText"].is_null()) {
            subtitleText = PsSubtitleOnScreenAttributesVo(data["subtitleText"]);
        }
    }
    
    nlohmann::json toJson() const {
        nlohmann::json result;
        result["subtitleText"] = subtitleText.toJson();
        return result;
    }
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
// BGM V2 structure matching Java BgmV2
struct BgmV2 {
    BgmV2() = default;
    BgmV2(const nlohmann::json& bgmJson) {
        if (bgmJson.contains("timelineUuid")) {
            timelineUuid = bgmJson["timelineUuid"].get<std::string>();
        }
        // Support legacy uuid field for backward compatibility
        if (bgmJson.contains("uuid") && timelineUuid.empty()) {
            timelineUuid = bgmJson["uuid"].get<std::string>();
        }
        if (bgmJson.contains("timeOffsetInProject")) {
            timeOffsetInProject = bgmJson["timeOffsetInProject"].get<int>();
        }
        if (bgmJson.contains("duration")) {
            duration = bgmJson["duration"].get<int>();
        }
        if (bgmJson.contains("assetUuid") || bgmJson.contains("asset_uuid")) {
            assetUuid = bgmJson.contains("assetUuid") ? 
                       bgmJson["assetUuid"].get<std::string>() : 
                       bgmJson["asset_uuid"].get<std::string>();
        }
        if (bgmJson.contains("assetLink")) {
            assetLink = bgmJson["assetLink"].get<std::string>();
        }
        if (bgmJson.contains("adjustedBgmLink")) {
            adjustedBgmLink = bgmJson["adjustedBgmLink"].get<std::string>();
        }
        if (bgmJson.contains("chorusStart")) {
            chorusStart = bgmJson["chorusStart"].get<int>();
        }
        if (bgmJson.contains("chorusEnd")) {
            chorusEnd = bgmJson["chorusEnd"].get<int>();
        }
        if (bgmJson.contains("mixed")) {
            mixed = bgmJson["mixed"];
        }
        if (bgmJson.contains("volume")) {
            volume = bgmJson["volume"].get<double>();
        }
    }
    
    std::string timelineUuid;  // Required - timeline uuid of the bgm
    std::optional<int> timeOffsetInProject;  // Bgm Time Offset in Project
    std::optional<int> duration;  // Bgm Timeline Duration
    std::optional<std::string> assetUuid;  // asset uuid of the bgm
    std::optional<std::string> assetLink;  // bgm link
    std::optional<std::string> adjustedBgmLink;  // adjusted bgm link
    std::optional<int> chorusStart;  // ai: chorus start time
    std::optional<int> chorusEnd;  // ai: chorus end time
    std::optional<nlohmann::json> mixed;  // bgm mix settings
    std::optional<double> volume;
    
    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["timelineUuid"] = timelineUuid;
        
        if (timeOffsetInProject.has_value()) {
            result["timeOffsetInProject"] = timeOffsetInProject.value();
        }
        if (duration.has_value()) {
            result["duration"] = duration.value();
        }
        if (assetUuid.has_value()) {
            result["assetUuid"] = assetUuid.value();
        }
        if (assetLink.has_value()) {
            result["assetLink"] = assetLink.value();
        }
        if (adjustedBgmLink.has_value()) {
            result["adjustedBgmLink"] = adjustedBgmLink.value();
        }
        if (chorusStart.has_value()) {
            result["chorusStart"] = chorusStart.value();
        }
        if (chorusEnd.has_value()) {
            result["chorusEnd"] = chorusEnd.value();
        }
        if (mixed.has_value()) {
            result["mixed"] = mixed.value();
        }
        if (volume.has_value()) {
            result["volume"] = volume.value();
        }
        
        return result;
    }
};

// Legacy alias for backward compatibility
using ProjectBgm = BgmV2;

// Synthetic voice metadata
struct SyntheticVoiceMetadata {
    SyntheticVoiceMetadata() = default;
    SyntheticVoiceMetadata(const nlohmann::json& voiceJson) {
        if (voiceJson.contains("uuid")) {
            voiceId = voiceJson["uuid"].get<std::string>();
        }
        if (voiceJson.contains("voiceId")) {
            voiceId = voiceJson["voiceId"].get<std::string>();
        }
        if (voiceJson.contains("voiceName")) {
            voiceName = voiceJson["voiceName"].get<std::string>();
        }
        if (voiceJson.contains("language")) {
            language = voiceJson["language"].get<std::string>();
        }
        if (voiceJson.contains("locale")) {
            language = voiceJson["locale"].get<std::string>();
        }
        if (voiceJson.contains("gender")) {
            gender = voiceJson["gender"].get<std::string>();
        }
        
        // Store additional parameters as JSON
        nlohmann::json additionalParams;
        if (voiceJson.contains("voiceSpeakerName")) {
            additionalParams["voiceSpeakerName"] = voiceJson["voiceSpeakerName"];
        }
        if (voiceJson.contains("duration")) {
            additionalParams["duration"] = voiceJson["duration"];
        }
        if (!additionalParams.empty()) {
            this->additionalParams = additionalParams;
        }
    }
    
    std::string voiceId;
    std::string voiceName;
    std::string language;
    std::string gender;
    std::optional<json> additionalParams;
    
    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["voiceId"] = voiceId;
        result["voiceName"] = voiceName;
        result["language"] = language;
        result["gender"] = gender;
        
        if (additionalParams.has_value()) {
            // 展开 additionalParams 到结果中
            for (const auto& [key, value] : additionalParams.value().items()) {
                result[key] = value;
            }
        }
        
        return result;
    }
};

// Enhanced scene structure
struct ExtendedProjectScene {
    // TranscriptModifiedStatus inner class matching Java
    struct TranscriptModifiedStatus {
        TranscriptModifiedStatus() = default;
        TranscriptModifiedStatus(bool changed, bool voiceRedo, bool recommendFootageRedo)
            : changed(changed), voiceRedo(voiceRedo), recommendFootageRedo(recommendFootageRedo) {}
        TranscriptModifiedStatus(const nlohmann::json& data) {
            if (data.contains("changed")) changed = data["changed"].get<bool>();
            if (data.contains("voiceRedo")) voiceRedo = data["voiceRedo"].get<bool>();
            if (data.contains("recommendFootageRedo")) recommendFootageRedo = data["recommendFootageRedo"].get<bool>();
        }
        
        bool changed{false};
        bool voiceRedo{false};
        bool recommendFootageRedo{false};
        
        nlohmann::json toJson() const {
            nlohmann::json result;
            result["changed"] = changed;
            result["voiceRedo"] = voiceRedo;
            result["recommendFootageRedo"] = recommendFootageRedo;
            return result;
        }
    };

    // 基本字段匹配 Java ProjectSceneVo
    std::string sceneUuid;  // 主字段名匹配 Java
    std::string projectUuid;
    std::string name;
    SceneTypeEnum sceneType{SceneTypeEnum::DEFAULT};
    std::optional<int> timeOffsetInProject;
    std::optional<int> duration;
    std::optional<int> audioFlag;
    std::optional<int> pauseTime{0};
    
    // Transcript 字段
    std::optional<SceneTranscript> transcript;
    
    // 时间线分类（匹配 Java ProjectSceneVo）
    std::vector<ExtendedTimeline> aRolls;
    std::vector<ExtendedTimeline> bRolls;
    std::vector<VoiceOver> voiceOvers;
    
    // 图层和效果（匹配 Java ProjectSceneVo）
    std::vector<BaseLayer> layers;  // 新图层系统
    std::vector<SceneTransition> transitions;
    std::optional<TextOnScreen> textOnScreen;
    
    // 已弃用字段（保持兼容性）
    std::optional<json> effect;     // 已弃用
    std::optional<json> scale;      // 已弃用
    std::optional<json> audio;      // 已弃用
    std::vector<json> layer;        // 已弃用图层
    
    // 转录修改状态 - 匹配 Java 的完整结构
    std::optional<bool> transcriptModified;
    std::optional<TranscriptModifiedStatus> transcriptModifiedStatus;
    
    // 策略和元数据
    int brollShorterPolicyKey{0};
    std::optional<double> bgmVolume;  // 对应 Java 的 BigDecimal bgmVolume
    
    // 兼容性字段：场景相关资产
    std::unordered_map<std::string, ProjectSceneAsset> assets; // key: asset_id
    
    // 内部字段（忽略）
    std::optional<long> id;       // JsonIgnore
    std::optional<long> projectId; // JsonIgnore

    ExtendedProjectScene() = default;
    ExtendedProjectScene(const nlohmann::json &_data) {
        // 基本字段匹配 Java ProjectSceneVo
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        // 向后兼容：如果 JSON 中使用的是 "uuid"，也能正确解析
        if (_data.contains("uuid") && !_data.contains("sceneUuid")) sceneUuid = _data["uuid"];
        
        if (_data.contains("projectUuid")) projectUuid = _data["projectUuid"];
        if (_data.contains("name")) name = _data["name"];
        if (_data.contains("sceneType")) sceneType = _data["sceneType"].get<SceneTypeEnum>();
        if (_data.contains("timeOffsetInProject")) timeOffsetInProject = _data["timeOffsetInProject"];
        if (_data.contains("duration")) duration = _data["duration"];
        if (_data.contains("audioFlag")) audioFlag = _data["audioFlag"];
        if (_data.contains("pauseTime")) pauseTime = _data["pauseTime"];
        
        // Transcript 字段
        if (_data.contains("transcript") && !_data["transcript"].is_null()) {
            transcript = SceneTranscript(_data["transcript"]);
        }
        
        // 时间线字段（匹配 Java 字段名）
        if (_data.contains("aRolls") && _data["aRolls"].is_array()) {
            for (const auto& arollJson : _data["aRolls"]) {
                aRolls.emplace_back(arollJson);
            }
        }
        // 向后兼容：支持旧的字段名
        if (_data.contains("arolls") && _data["arolls"].is_array()) {
            for (const auto& arollJson : _data["arolls"]) {
                aRolls.emplace_back(arollJson);
            }
        }
        
        if (_data.contains("bRolls") && _data["bRolls"].is_array()) {
            for (const auto& brollJson : _data["bRolls"]) {
                bRolls.emplace_back(brollJson);
            }
        }
        // 向后兼容：支持旧的字段名
        if (_data.contains("brolls") && _data["brolls"].is_array()) {
            for (const auto& brollJson : _data["brolls"]) {
                bRolls.emplace_back(brollJson);
            }
        }
        
        if (_data.contains("voiceOvers") && _data["voiceOvers"].is_array()) {
            for (const auto& voJson : _data["voiceOvers"]) {
                voiceOvers.emplace_back(voJson);
            }
        }
        
        // 图层和效果字段
        if (_data.contains("layers") && _data["layers"].is_array()) {
            // TODO: 解析 layers
        }
        if (_data.contains("transitions") && _data["transitions"].is_array()) {
            for (const auto& transJson : _data["transitions"]) {
                transitions.emplace_back(transJson);
            }
        }
        if (_data.contains("textOnScreen") && !_data["textOnScreen"].is_null()) {
            textOnScreen = TextOnScreen(_data["textOnScreen"]);
        }
        
        // 已弃用字段
        if (_data.contains("effect")) {
            effect = _data["effect"];
        }
        if (_data.contains("scale")) {
            scale = _data["scale"];
        }
        if (_data.contains("audio")) {
            audio = _data["audio"];
        }
        if (_data.contains("layer") && _data["layer"].is_array()) {
            layer = _data["layer"];
        }
        
        // 其他字段
        if (_data.contains("transcriptModified")) {
            transcriptModified = _data["transcriptModified"];
        }
        if (_data.contains("transcriptModifiedStatus") && !_data["transcriptModifiedStatus"].is_null()) {
            transcriptModifiedStatus = TranscriptModifiedStatus(_data["transcriptModifiedStatus"]);
        }
        if (_data.contains("brollShorterPolicyKey")) {
            brollShorterPolicyKey = _data["brollShorterPolicyKey"];
        }
        if (_data.contains("bgmVolume")) {
            bgmVolume = _data["bgmVolume"];
        }
        
        // 资产字段
        if (_data.contains("assets") && _data["assets"].is_object()) {
            for (const auto& [assetId, assetJson] : _data["assets"].items()) {
                assets[assetId] = ProjectSceneAsset(assetJson);
            }
        }
    };

    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        // 基本字段
        result["sceneUuid"] = sceneUuid;
        result["projectUuid"] = projectUuid;
        result["name"] = name;
        result["sceneType"] = static_cast<int>(sceneType);
        
        // Optional 字段
        if (timeOffsetInProject.has_value()) result["timeOffsetInProject"] = timeOffsetInProject.value();
        if (duration.has_value()) result["duration"] = duration.value();
        if (audioFlag.has_value()) result["audioFlag"] = audioFlag.value();
        if (pauseTime.has_value()) result["pauseTime"] = pauseTime.value();
        
        // Transcript 字段 - 简化版
        if (transcript.has_value()) {
            result["transcript"] = transcript->toJson();
        }
        
        // 时间线数组 - 使用真实的序列化（匹配 Java 字段名）
        result["aRolls"] = nlohmann::json::array();
        for (const auto& aroll : aRolls) {
            result["aRolls"].push_back(aroll.toJson());
        }
        
        result["bRolls"] = nlohmann::json::array();
        for (const auto& broll : bRolls) {
            result["bRolls"].push_back(broll.toJson());
        }
        
        result["voiceOvers"] = nlohmann::json::array();
        for (const auto& voiceOver : voiceOvers) {
            result["voiceOvers"].push_back(voiceOver.toJson());
        }
        
        // 图层和效果 - 使用真实的序列化
        result["layers"] = nlohmann::json::array();
        for (const auto& layer : layers) {
            result["layers"].push_back(layer.toJson());
        }
        
        result["transitions"] = nlohmann::json::array();
        for (const auto& transition : transitions) {
            result["transitions"].push_back(transition.toJson());
        }
        
        // TextOnScreen 字段 - 使用真实的序列化
        if (textOnScreen.has_value()) {
            result["textOnScreen"] = textOnScreen->toJson();
        }
        
        // 已弃用字段
        if (effect.has_value()) result["effect"] = effect.value();
        if (scale.has_value()) result["scale"] = scale.value();
        if (audio.has_value()) result["audio"] = audio.value();
        if (!layer.empty()) result["layer"] = layer;
        
        // 其他字段
        if (transcriptModified.has_value()) result["transcriptModified"] = transcriptModified.value();
        if (transcriptModifiedStatus.has_value()) result["transcriptModifiedStatus"] = transcriptModifiedStatus->toJson();
        result["brollShorterPolicyKey"] = brollShorterPolicyKey;
        if (bgmVolume.has_value()) result["bgmVolume"] = bgmVolume.value();
        
        // 资产 - 使用真实的序列化
        if (!assets.empty()) {
            result["assets"] = nlohmann::json::object();
            for (const auto& [assetId, asset] : assets) {
                result["assets"][assetId] = asset.toJson();
            }
        }
        
        return result;
    }
    
    // Getter 方法匹配 Java 行为
    std::string getSceneUuid() const {
        return id.has_value() ? std::to_string(id.value()) : sceneUuid;
    }
    
    std::string getProjectUuid() const {
        return projectId.has_value() ? std::to_string(projectId.value()) : projectUuid;
    }
    
    // TranscriptModified setter method matching Java behavior
    void setTranscriptModified(TranscriptModifiedStatusEnum transcriptModifiedStatusEnum) {
        switch (transcriptModifiedStatusEnum) {
            case TranscriptModifiedStatusEnum::NO_CHANGED:
                this->transcriptModified = false;
                this->transcriptModifiedStatus = TranscriptModifiedStatus(false, false, false);
                break;
            case TranscriptModifiedStatusEnum::CHANGED:
                this->transcriptModified = true;
                this->transcriptModifiedStatus = TranscriptModifiedStatus(true, false, false);
                break;
            case TranscriptModifiedStatusEnum::SYNTHETIC_REDO:
                this->transcriptModified = true;
                this->transcriptModifiedStatus = TranscriptModifiedStatus(true, true, false);
                break;
            case TranscriptModifiedStatusEnum::RECOMMEND_FOOTAGE_REDO:
                this->transcriptModified = true;
                this->transcriptModifiedStatus = TranscriptModifiedStatus(true, false, true);
                break;
            case TranscriptModifiedStatusEnum::ALL_REDO:
                this->transcriptModified = false;
                this->transcriptModifiedStatus = TranscriptModifiedStatus(false, true, true);
                break;
            default:
                this->transcriptModified = false;
                this->transcriptModifiedStatus = TranscriptModifiedStatus(false, false, false);
                break;
        }
    }
    
    // 向后兼容性方法：获取 uuid（原字段名）
    std::string getUuid() const {
        return getSceneUuid();
    }
    
    // 向后兼容性方法：设置 uuid（原字段名）
    void setUuid(const std::string& value) {
        sceneUuid = value;
    }
    
    // Legacy compatibility - unified timelines
    std::vector<ExtendedTimeline> timelines; // computed from aRolls + bRolls + voiceOvers
};

// Enhanced project structure
// Project metadata structure
struct ExtendedProject {
    ExtendedProject() = default;
    ExtendedProject(const nlohmann::json& data) {
        if (data.contains("uuid")) {
            uuid = data["uuid"].get<std::string>();
        }
        if (data.contains("name")) {
            name = data["name"].get<std::string>();
        }
        if (data.contains("ownerUuid")) {
            ownerUuid = data["ownerUuid"].get<std::string>();
        }
        if (data.contains("status")) {
            std::string statusStr = data["status"].get<std::string>();
            if (statusStr == "ACTIVE") status = StatusEnum::ACTIVE;
            else if (statusStr == "DELETED") status = StatusEnum::DELETED;
            else if (statusStr == "DRAFT") status = StatusEnum::DRAFT;
        }
        if (data.contains("padColor")) {
            padColor = data["padColor"].get<std::string>();
        }
        if (data.contains("videoFormat")) {
            videoFormat = data["videoFormat"].get<int>();
        }
        if (data.contains("brollShorterPolicyKey")) {
            brollShorterPolicyKey = data["brollShorterPolicyKey"].get<int>();
        }
        if (data.contains("syntheticAll")) {
            syntheticAll = data["syntheticAll"].get<bool>();
        }
        if (data.contains("version")) {
            version = data["version"].get<int>();
        }
    }
    
    std::string uuid;
    std::string name;
    std::string ownerUuid;
    StatusEnum status{StatusEnum::ACTIVE};
    std::optional<std::string> padColor;
    int videoFormat{0};
    int brollShorterPolicyKey{0};
    std::optional<bool> syntheticAll;
    int version{1};
    
    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["uuid"] = uuid;
        result["name"] = name;
        result["ownerUuid"] = ownerUuid;
        result["version"] = version;
        
        // 状态转换
        switch (status) {
            case StatusEnum::ACTIVE: result["status"] = "ACTIVE"; break;
            case StatusEnum::DELETED: result["status"] = "DELETED"; break;
            case StatusEnum::DRAFT: result["status"] = "DRAFT"; break;
        }
        
        // Optional 字段
        if (padColor.has_value()) {
            result["padColor"] = padColor.value();
        }
        result["videoFormat"] = videoFormat;
        result["brollShorterPolicyKey"] = brollShorterPolicyKey;
        if (syntheticAll.has_value()) {
            result["syntheticAll"] = syntheticAll.value();
        }
        
        return result;
    }
};

struct ExtendedProjectAndScenesVo {
    // 基本项目信息 - 匹配 Java ProjectAndScenesVo
    std::string projectUuid;
    std::string ownerUuid;
    StatusEnum status{StatusEnum::ACTIVE};
    
    // Content - 匹配 Java 字段
    std::vector<ProjectBgm> bgms;
    std::vector<ExtendedProjectScene> scenes;
    std::unordered_map<std::string, ProjectSceneAsset> assets; // assetId -> asset
    std::unordered_map<std::string, SyntheticVoiceMetadata> syntheticVoices; // assetId -> voice
    
    // 新增字段匹配 Java ProjectAndScenesVo
    bool syntheticAll{false};
    std::optional<std::string> padColor;
    int videoFormat{0};
    int brollShorterPolicyKey{0};
    
    // Style 字段
    std::optional<json> text; // 文本样式
    std::optional<json> style; // 样式配置
    
    // 兼容性字段
    ExtendedProject project; // 保持向后兼容
    std::vector<ExtendedTimeline> timelines; // Global timeline list for backward compatibility
    int version{1}; // for conflict detection

    public:
        ExtendedProjectAndScenesVo() = default;
        ExtendedProjectAndScenesVo(const nlohmann::json &data) {
            if (data.contains("projectUuid")) {
                projectUuid = data["projectUuid"].get<std::string>();
            }
            if (data.contains("ownerUuid")) {
                ownerUuid = data["ownerUuid"].get<std::string>();
            }
            if (data.contains("status")) {
                std::string statusStr = data["status"].get<std::string>();
                if (statusStr == "ACTIVE") status = StatusEnum::ACTIVE;
                else if (statusStr == "DELETED") status = StatusEnum::DELETED;
            }
            
            // 解析 bgms
            if (data.contains("bgms") && data["bgms"].is_array()) {
                for (const auto& bgmJson : data["bgms"]) {
                    bgms.emplace_back(bgmJson);
                }
            }
            
            // 解析 scenes
            if (data.contains("scenes") && data["scenes"].is_array()) {
                for (const auto& sceneJson : data["scenes"]) {
                    scenes.emplace_back(sceneJson);
                }
            }
            
            // 解析 assets
            if (data.contains("assets") && data["assets"].is_object()) {
                for (const auto& [assetId, assetJson] : data["assets"].items()) {
                    assets[assetId] = ProjectSceneAsset(assetJson);
                }
            }
            
            // 解析 syntheticVoices
            if (data.contains("syntheticVoices") && data["syntheticVoices"].is_object()) {
                for (const auto& [voiceId, voiceJson] : data["syntheticVoices"].items()) {
                    syntheticVoices[voiceId] = SyntheticVoiceMetadata(voiceJson);
                }
            }
            
            // 新增字段
            if (data.contains("syntheticAll")) {
                syntheticAll = data["syntheticAll"].get<bool>();
            }
            if (data.contains("padColor")) {
                padColor = data["padColor"].get<std::string>();
            }
            if (data.contains("videoFormat")) {
                videoFormat = data["videoFormat"].get<int>();
            }
            if (data.contains("brollShorterPolicyKey")) {
                brollShorterPolicyKey = data["brollShorterPolicyKey"].get<int>();
            }
            
            // Style 字段
            if (data.contains("text")) {
                text = data["text"];
            }
            if (data.contains("style")) {
                style = data["style"];
            }
            
            // 兼容性字段
            if (data.contains("project")) {
                project = ExtendedProject(data["project"]);
            }
            if (data.contains("version")) {
                version = data["version"].get<int>();
            }
        }

        // 添加 toJson 方法用于序列化
        nlohmann::json toJson() const {
            nlohmann::json result;
            
            result["projectUuid"] = projectUuid;
            result["ownerUuid"] = ownerUuid;
            
            // 状态转换
            switch (status) {
                case StatusEnum::ACTIVE: result["status"] = "ACTIVE"; break;
                case StatusEnum::DELETED: result["status"] = "DELETED"; break;
                case StatusEnum::DRAFT: result["status"] = "DRAFT"; break;
            }
            
            // BGMs 数组 - 使用真实的序列化
            result["bgms"] = nlohmann::json::array();
            for (const auto& bgm : bgms) {
                result["bgms"].push_back(bgm.toJson());
            }
            
            // Scenes 数组
            result["scenes"] = nlohmann::json::array();
            for (const auto& scene : scenes) {
                result["scenes"].push_back(scene.toJson());
            }
            
            // Assets 对象 - 使用真实的序列化
            result["assets"] = nlohmann::json::object();
            for (const auto& [assetId, asset] : assets) {
                result["assets"][assetId] = asset.toJson();
            }
            
            // Synthetic voices 对象 - 使用真实的序列化
            result["syntheticVoices"] = nlohmann::json::object();
            for (const auto& [voiceId, voice] : syntheticVoices) {
                result["syntheticVoices"][voiceId] = voice.toJson();
            }
            
            // 新增字段
            result["syntheticAll"] = syntheticAll;
            if (padColor.has_value()) result["padColor"] = padColor.value();
            result["videoFormat"] = videoFormat;
            result["brollShorterPolicyKey"] = brollShorterPolicyKey;
            
            // Style 字段
            if (text.has_value()) result["text"] = text.value();
            if (style.has_value()) result["style"] = style.value();
            
            // 兼容性字段 - 使用真实的序列化
            result["project"] = project.toJson();
            result["version"] = version;
            
            return result;
        }
};

// Single scene VO structure (matching Java ProjectAndSceneVo)
struct ExtendedProjectAndSceneVo {
    ExtendedProjectAndSceneVo() = default;
    ExtendedProjectAndSceneVo(const nlohmann::json& data) {
        if (data.contains("projectUuid")) {
            projectUuid = data["projectUuid"].get<std::string>();
        }
        if (data.contains("sceneUuid")) {
            sceneUuid = data["sceneUuid"].get<std::string>();
        }
        if (data.contains("scene")) {
            scene = ExtendedProjectScene(data["scene"]);
        }
        if (data.contains("assets") && data["assets"].is_object()) {
            for (const auto& [assetId, assetJson] : data["assets"].items()) {
                ProjectSceneAsset asset(assetJson);
                assets[assetId] = asset;
            }
        }
    }
    
    std::string projectUuid;
    std::string sceneUuid;
    std::optional<ExtendedProjectScene> scene;
    std::unordered_map<std::string, ProjectSceneAsset> assets; // key: asset_id
    
    // Getter methods matching Java behavior
    std::string getProjectUuid() const {
        return scene.has_value() ? scene->projectUuid : projectUuid;
    }
    
    std::string getSceneUuid() const {
        return scene.has_value() ? scene->sceneUuid : sceneUuid;
    }

    // 添加 toJson 方法用于序列化
    nlohmann::json toJson() const {
        nlohmann::json result;
        
        result["projectUuid"] = getProjectUuid();
        result["sceneUuid"] = getSceneUuid();
        
        if (scene.has_value()) {
            result["scene"] = scene->toJson();
        }
        
        // Assets 对象 - 使用真实的序列化
        result["assets"] = nlohmann::json::object();
        for (const auto& [assetId, asset] : assets) {
            result["assets"][assetId] = asset.toJson();
        }
        
        return result;
    }
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
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("newIndex")) newIndex = _data["newIndex"];
        if (_data.contains("afterSceneUuid")) afterSceneUuid = _data["afterSceneUuid"];
    }
    std::string sceneUuid; 
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
// Scene Roll V2 structure to match Java SceneRollV2
struct SceneRollV2 {
    SceneRollV2() = default;
    SceneRollV2(const nlohmann::json &_data) {
        if (_data.contains("entityType")) entityType = static_cast<EntityTypeEnum>(_data["entityType"]);
        if (_data.contains("entityUuid")) entityUuid = _data["entityUuid"];
        if (_data.contains("providerId")) providerId = _data["providerId"];
        if (_data.contains("timeOffsetInProject")) timeOffsetInProject = _data["timeOffsetInProject"];
        if (_data.contains("startTime")) startTime = _data["startTime"];
        if (_data.contains("endTime")) endTime = _data["endTime"];
    }
    
    EntityTypeEnum entityType{EntityTypeEnum::PROJECT_ASSET};
    std::string entityUuid;
    std::optional<int> providerId;
    std::optional<int> timeOffsetInProject;
    std::optional<int> startTime;
    std::optional<int> endTime;
    
    int getDuration() const {
        if (!startTime.has_value() || !endTime.has_value()) {
            return 0;
        }
        return endTime.value() - startTime.value();
    }
};

struct ProjectSceneFootageAddReqBody {
    ProjectSceneFootageAddReqBody() = default;
    ProjectSceneFootageAddReqBody(const nlohmann::json &_data) {
        if (_data.contains("sceneUuid")) sceneUuid = _data["sceneUuid"];
        if (_data.contains("bRoll")) bRoll = SceneRollV2(_data["bRoll"]);
        if (_data.contains("recordVideoOver")) recordVideoOver = _data["recordVideoOver"];
    }
    std::string sceneUuid;
    SceneRollV2 bRoll;
    bool recordVideoOver{false};
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
// BGM V2 Request Bodies matching Java API
struct BgmV2AddReqBody {
    BgmV2AddReqBody() = default;
    BgmV2AddReqBody(const nlohmann::json &_data) {
        if (_data.contains("assetUuid")) assetUuid = _data["assetUuid"];
        if (_data.contains("startSceneIndex")) startSceneIndex = _data["startSceneIndex"];
        if (_data.contains("endSceneIndex")) endSceneIndex = _data["endSceneIndex"];
        if (_data.contains("volume")) volume = _data["volume"];
        if (_data.contains("timeOffsetInProject")) timeOffsetInProject = _data["timeOffsetInProject"];
        if (_data.contains("duration")) duration = _data["duration"];
        if (_data.contains("chorusStart")) chorusStart = _data["chorusStart"];
        if (_data.contains("chorusEnd")) chorusEnd = _data["chorusEnd"];
    }
    std::string assetUuid;
    std::optional<int> startSceneIndex;
    std::optional<int> endSceneIndex;
    std::optional<double> volume;
    std::optional<int> timeOffsetInProject;
    std::optional<int> duration;
    std::optional<int> chorusStart;
    std::optional<int> chorusEnd;
};

struct BgmV2DeleteReqBody {
    BgmV2DeleteReqBody() = default;
    BgmV2DeleteReqBody(const nlohmann::json &_data) {
        if (_data.contains("timelineUuid")) timelineUuid = _data["timelineUuid"];
    }
    std::string timelineUuid;
};

struct BgmV2EditReqBody {
    BgmV2EditReqBody() = default;
    BgmV2EditReqBody(const nlohmann::json &_data) {
        if (_data.contains("timelineUuid")) timelineUuid = _data["timelineUuid"];
        if (_data.contains("volume")) volume = _data["volume"];
        if (_data.contains("timeOffsetInProject")) timeOffsetInProject = _data["timeOffsetInProject"];
        if (_data.contains("duration")) duration = _data["duration"];
        if (_data.contains("chorusStart")) chorusStart = _data["chorusStart"];
        if (_data.contains("chorusEnd")) chorusEnd = _data["chorusEnd"];
        if (_data.contains("assetUuid")) assetUuid = _data["assetUuid"];
    }
    std::string timelineUuid;
    std::optional<double> volume;
    std::optional<int> timeOffsetInProject;
    std::optional<int> duration;
    std::optional<int> chorusStart;
    std::optional<int> chorusEnd;
    std::optional<std::string> assetUuid;
};

// Legacy aliases for backward compatibility
using ProjectBgmAddReqBody = BgmV2AddReqBody;
using ProjectBgmDeleteReqBody = BgmV2DeleteReqBody;
using ProjectBgmEditReqBody = BgmV2EditReqBody;

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
    {ProjectTimelineCategoryEnum::NARRATION_VOICE_OVER, "NARRATION_VOICE_OVER"},
    {ProjectTimelineCategoryEnum::BACKGROUND_MUSIC, "BACKGROUND_MUSIC"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(StatusEnum, {
    {StatusEnum::ACTIVE, "ACTIVE"},
    {StatusEnum::DELETED, "DELETED"},
    {StatusEnum::DRAFT, "DRAFT"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(EntityTypeEnum, {
    {EntityTypeEnum::UNKNOWN, "unknown"},
    {EntityTypeEnum::USER, "user"},
    {EntityTypeEnum::CLIP, "clip"},
    {EntityTypeEnum::FOLDER, "folder"},
    {EntityTypeEnum::PROJECT, "project"},
    {EntityTypeEnum::PROJECT_ASSET, "projectAsset"},
    {EntityTypeEnum::PROJECT_SEQUENCE, "sequence"},
    {EntityTypeEnum::PROJECT_VIDEO, "project_video"},
    {EntityTypeEnum::STOCK_MEDIA, "stock_media"},
    {EntityTypeEnum::STOCK_AUDIO, "stock_audio"},
    {EntityTypeEnum::VOICE, "voice"},
    {EntityTypeEnum::PICTURES, "pictures"},
    {EntityTypeEnum::VISLA_MEDIA, "visla_media"},
    {EntityTypeEnum::STOCK_MEDIA_PEXELS, "pexels"},
    {EntityTypeEnum::STOCK_MEDIA_STORYBLOCKS, "storyblocks"},
    {EntityTypeEnum::STOCK_MEDIA_GETTYIMAGES, "gettyimages"},
    {EntityTypeEnum::AGENT_PROJECT, "agent_project"},
    {EntityTypeEnum::AGENT_PROJECT_MEDIA, "agent_project_media"},
    {EntityTypeEnum::CUSTOM_VOICE, "custom_voice"},
    {EntityTypeEnum::AVATAR, "avatar"},
    {EntityTypeEnum::AVATAR_LOOK, "avatar_look"},
    {EntityTypeEnum::WORKSPACE, "workspace"},
    {EntityTypeEnum::TEAMSPACE, "teamspace"}
})

// Scale-related structures (matching Java PsScaleParamsBo, PsTimelineScaleBo, PsSceneScaleReqBody)
struct PsScaleParams {
    double value{1.0};
    std::optional<std::vector<double>> coordOffset;
    
    PsScaleParams() = default;
    PsScaleParams(const nlohmann::json& data) {
        if (data.contains("value") && !data["value"].is_null()) {
            value = data["value"].get<double>();
        }
        if (data.contains("coordOffset") && !data["coordOffset"].is_null()) {
            coordOffset = data["coordOffset"].get<std::vector<double>>();
        }
    }
};

struct PsTimelineScale {
    std::string timelineUuid;
    PsScaleParams scale;
    
    PsTimelineScale() = default;
    PsTimelineScale(const nlohmann::json& data) {
        if (data.contains("timelineUuid") && !data["timelineUuid"].is_null()) {
            timelineUuid = data["timelineUuid"].get<std::string>();
        }
        if (data.contains("scale") && !data["scale"].is_null()) {
            scale = PsScaleParams(data["scale"]);
        }
    }
};

struct PsSceneScaleReqBody {
    std::string sceneUuid;
    std::vector<PsTimelineScale> scales;
    
    PsSceneScaleReqBody() = default;
    PsSceneScaleReqBody(const nlohmann::json& data) {
        if (data.contains("sceneUuid") && !data["sceneUuid"].is_null()) {
            sceneUuid = data["sceneUuid"].get<std::string>();
        }
        if (data.contains("scales") && data["scales"].is_array()) {
            for (const auto& scaleData : data["scales"]) {
                scales.emplace_back(PsTimelineScale(scaleData));
            }
        }
    }
};

// Response structures
struct PsTimelineScaleVo {
    std::string timelineId;
    PsScaleParams scale;
};

struct PsSceneScaleVo {
    std::string sceneUuid;
    std::vector<PsTimelineScaleVo> scales;
};

} // namespace pjson

#endif // PJSON_EDITOR_EXTENDED_MODELS_H
