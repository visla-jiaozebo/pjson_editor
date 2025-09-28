#ifndef PJSON_EDITOR_TRANSCRIPT_UTILS_H
#define PJSON_EDITOR_TRANSCRIPT_UTILS_H

#include "ExtendedModels.h"
#include <utility>
#include <string>

namespace pjson {

struct TranscriptSplitResult { SceneTranscript first; SceneTranscript second; };

// Simple helper to split a transcript at splitMs proportionally.
// TODO: Implement pace-based, word-boundary aware splitting matching backend TranscriptUtil.
inline TranscriptSplitResult splitTranscriptSimple(const SceneTranscript &original, int splitMs, int sceneDuration) {
    TranscriptSplitResult result;
    if (sceneDuration <= 0 || splitMs <= 0 || splitMs >= sceneDuration) {
        result.first = original; // No actual split possible
        return result;
    }
    double ratio = static_cast<double>(splitMs) / static_cast<double>(sceneDuration);
    size_t splitPoint = static_cast<size_t>(original.text.size() * ratio);
    while (splitPoint < original.text.size() && original.text[splitPoint] != ' ') {
        ++splitPoint; // move to next boundary (naive)
    }
    result.first.text = original.text.substr(0, splitPoint);
    result.second.text = splitPoint < original.text.size() ? original.text.substr(splitPoint) : std::string();
    result.first.modified = result.second.modified = false; // caller may override
    return result;
}

// Simple merge: concatenate with a space. TODO: adopt backend timing & item merge rules.
inline SceneTranscript mergeTranscriptSimple(const SceneTranscript &a, const SceneTranscript &b) {
    SceneTranscript merged;
    merged.text = a.text;
    if (!merged.text.empty() && !b.text.empty()) merged.text += ' ';
    merged.text += b.text;
    merged.modified = false; // caller may override
    return merged;
}

} // namespace pjson

#endif // PJSON_EDITOR_TRANSCRIPT_UTILS_H
