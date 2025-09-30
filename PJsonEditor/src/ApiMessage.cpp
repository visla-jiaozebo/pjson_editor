#include "pjson_editor/ApiMessage.h"

namespace pjson {

std::unordered_map<ApiMessage, std::pair<int, std::string>> ApiMessageHelper::messageMap;

void ApiMessageHelper::initialize() {
    if (!messageMap.empty()) {
        return; // Already initialized
    }
    
    messageMap[ApiMessage::SUCCESS] = {0, "SUCCESS"};
    
    // UNKNOWN(1)
    messageMap[ApiMessage::UNKNOWN_ERROR] = {1000, "Unknown error"};
    
    // INVALID_ARGUMENT(3)
    messageMap[ApiMessage::ILLEGAL_PARAMS] = {3000, "Invalid request parameters"};
    messageMap[ApiMessage::MISSING_PARAMS] = {3003, "Missing required parameters"};
    messageMap[ApiMessage::INVALID_PROJECT_SPLIT_SCENE] = {3019, "We are unable to refine your videos. "};
    messageMap[ApiMessage::SCENE_CUT_DURATION_LIMITATION] = {3027, "The scene duration cannot be shorter than 1 second."};
    messageMap[ApiMessage::PROJECT_SCENE_STORY_AUDIO_EXIST] = {3032, "The Scene already has an audio. Please remove the audio before adding a new one."};
    
    // NOT_FOUND(4)
    messageMap[ApiMessage::PROJECT_NOT_FOUND] = {4000, "Project is not found"};
    messageMap[ApiMessage::PROJECT_VIDEO_SCENES_NOT_FOUND] = {4021, "Project video scenes is not found"};
    messageMap[ApiMessage::PROJECT_VIDEO_SCENE_NOT_FOUND] = {4024, "Scene is not found"};
    messageMap[ApiMessage::PROJECT_SCENES_NOT_FOUND] = {4046, "Project scenes is not found"};
    messageMap[ApiMessage::PROJECT_SCENE_SCRIPT_EMPTY] = {4053, "The project scene script is empty."};
    messageMap[ApiMessage::PROJECT_AVATAR_NOT_FOUND] = {4064, "The project scene avatar is not found."};
    
    // CONFLICT(5)
    messageMap[ApiMessage::PROJECT_SCENE_EXIST_VOICE_OVER] = {5016, "The scene have voice over already."};
    
    // PERMISSION_DENIED(6)
    messageMap[ApiMessage::ACL_ACCESS_DENIED] = {6002, "Access denied"};
    messageMap[ApiMessage::ACTION_DENIED] = {6004, "Action denied"};
    messageMap[ApiMessage::MUST_AT_LEAST_ONE_SCENE] = {6010, "There must be at least one selected scene"};
    messageMap[ApiMessage::TRANSITION_NOT_SUPPORT_ON_LAST_SCENE] = {6022, "We do not support transition on last scene"};
    messageMap[ApiMessage::SPLIT_NOT_SUPPORT_ON_EMPTY_SCENE] = {6026, "Scene split not support due to empty script."};
    messageMap[ApiMessage::INTRO_OUTRO_ASSET_NOT_ALLOWED] = {6028, "Recording asset can't set as intro/outro."};
    
    // DataStore specific (custom)
    messageMap[ApiMessage::DATASTORE_NOT_INITIALIZED] = {9000, "DataStore not initialized"};
}

int ApiMessageHelper::getCode(ApiMessage message) {
    initialize();
    auto it = messageMap.find(message);
    return it != messageMap.end() ? it->second.first : static_cast<int>(ApiMessage::UNKNOWN_ERROR);
}

std::string ApiMessageHelper::getMessage(ApiMessage message) {
    initialize();
    auto it = messageMap.find(message);
    return it != messageMap.end() ? it->second.second : "Unknown error";
}

std::pair<int, std::string> ApiMessageHelper::getCodeAndMessage(ApiMessage message) {
    initialize();
    auto it = messageMap.find(message);
    return it != messageMap.end() ? it->second : std::make_pair(static_cast<int>(ApiMessage::UNKNOWN_ERROR), "Unknown error");
}

} // namespace pjson