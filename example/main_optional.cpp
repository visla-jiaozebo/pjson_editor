#include <iostream>

#ifdef HAVE_PJSONEDITOR
#include <pjson_editor/ExtendedAPI.h>
#include <pjson_editor/ExtendedModels.h>
#include <pjson_editor/TranscriptUtils.h>
#include <nlohmann/json.hpp>
#endif

int main() {
    std::cout << "PJsonEditor Example" << std::endl;
    
#ifdef HAVE_PJSONEDITOR
    std::cout << "✓ Successfully linked with PJsonEditor library!" << std::endl;
    
    // Demonstrate that nlohmann_json is also available
    nlohmann::json test_json;
    test_json["message"] = "PJsonEditor and nlohmann_json are working!";
    std::cout << "✓ JSON test: " << test_json.dump() << std::endl;
    
    // Add your PJsonEditor usage here
    // This is just a basic example to verify linking works
#else
    std::cout << "⚠ Built without PJsonEditor support" << std::endl;
#endif
    
    return 0;
}