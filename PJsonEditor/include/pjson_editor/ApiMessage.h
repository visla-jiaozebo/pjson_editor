#ifndef PJSON_EDITOR_API_MESSAGE_H
#define PJSON_EDITOR_API_MESSAGE_H

#include <string>
#include <unordered_map>

namespace pjson {

/**
 * Error messages grouped under error code
 * C++ equivalent of Java ApiMessage enum
 */
enum class ApiMessage {
    // Success
    SUCCESS = 0,
    
    // Error messages grouped under error code: UNKNOWN(1)
    UNKNOWN_ERROR = 1000,
    
    // Error messages grouped under error code: INVALID_ARGUMENT(3)
    ILLEGAL_PARAMS = 3000,
    MISSING_PARAMS = 3003,
    INVALID_PROJECT_SPLIT_SCENE = 3019,
    SCENE_CUT_DURATION_LIMITATION = 3027,
    PROJECT_SCENE_STORY_AUDIO_EXIST = 3032,
    
    // Error messages grouped under error code: NOT_FOUND(4)
    PROJECT_NOT_FOUND = 4000,
    PROJECT_VIDEO_SCENES_NOT_FOUND = 4021,
    PROJECT_VIDEO_SCENE_NOT_FOUND = 4024,
    PROJECT_SCENES_NOT_FOUND = 4046,
    PROJECT_SCENE_SCRIPT_EMPTY = 4053,
    PROJECT_AVATAR_NOT_FOUND = 4064,
    
    // Error messages grouped under error code: CONFLICT(5)
    PROJECT_SCENE_EXIST_VOICE_OVER = 5016,
    
    // Error messages grouped under error code: PERMISSION_DENIED(6)
    ACL_ACCESS_DENIED = 6002,
    ACTION_DENIED = 6004,
    MUST_AT_LEAST_ONE_SCENE = 6010,
    TRANSITION_NOT_SUPPORT_ON_LAST_SCENE = 6022,
    SPLIT_NOT_SUPPORT_ON_EMPTY_SCENE = 6026,
    INTRO_OUTRO_ASSET_NOT_ALLOWED = 6028,
    
    // DataStore specific errors (custom)
    DATASTORE_NOT_INITIALIZED = 9000,
};

class ApiMessageHelper {
private:
    static std::unordered_map<ApiMessage, std::pair<int, std::string>> messageMap;
    
public:
    /**
     * Get error code for the given ApiMessage
     */
    static int getCode(ApiMessage message);
    
    /**
     * Get error message text for the given ApiMessage
     */
    static std::string getMessage(ApiMessage message);
    
    /**
     * Get both code and message as a pair
     */
    static std::pair<int, std::string> getCodeAndMessage(ApiMessage message);
    
    /**
     * Initialize the message map (called automatically)
     */
    static void initialize();
};

} // namespace pjson

#endif // PJSON_EDITOR_API_MESSAGE_H