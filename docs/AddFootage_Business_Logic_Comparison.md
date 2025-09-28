# PJsonEditor addFootage 与 Java addSceneFootage 业务逻辑对比分析

## 问题总结

PJsonEditor的`addFootage`实现与Java工程的`addSceneFootage`在业务逻辑上存在显著差异，主要缺少了Java后端的核心业务逻辑。

## 详细差异对比

### 1. 权限验证 ❌→✅
**Java 实现:**
```java
ProjectService.Helper helper = ProjectService.Helper.initAndCheckProjectExisting(projectUuid);
if (!helper.isProjectEditPermission()) {
    throw new ApiException(ApiMessage.ACL_ACCESS_DENIED);
}
```

**C++ 原实现:** 无权限检查
**C++ 修复后:** 仍需要添加权限检查框架

### 2. 场景类型过滤 ❌→✅ 
**Java 实现:**
```java
filterIntroOutroScene(matchedScene); // 禁止在intro/outro场景添加footage
```

**C++ 原实现:** 无场景类型检查
**C++ 修复后:** 
```cpp
if (scene->sceneType == SceneTypeEnum::INTRO || scene->sceneType == SceneTypeEnum::OUTRO) {
    return ApiResult(false, "Cannot add footage to intro/outro scenes");
}
```

### 3. 时间线重叠检查 ❌→✅
**Java 实现:**
```java
avoidOverlapSceneTimeline(project, matchedScene, bRoll.getTimeOffsetInProject(), bRoll.getDuration(), footageTimelines);
```

**C++ 原实现:** 无重叠检查
**C++ 修复后:** 添加了基础重叠检查逻辑

### 4. 语音轨道检测 ❌→✅
**Java 实现:**
```java
List<ProjectTimeline> voiceOverLines = timelines.stream()
    .filter(tl -> Objects.equals(SYNTHETIC_VOICE_OVER, tl.getCategory()) || 
                  Objects.equals(RECORD_VOICE_OVER, tl.getCategory()) ||
                  Objects.equals(STORY_AUDIO, tl.getCategory()) || 
                  Objects.equals(MAIN_STORY, tl.getCategory())).collect(Collectors.toList());
```

**C++ 原实现:** 无语音轨道检测
**C++ 修复后:** 添加了语音轨道收集和检测逻辑

### 5. 复杂的endTime计算 ❌→✅ 
**Java 实现:**
```java
int end = 0;
if (VideoCreationTemplateEnum.VIDEO_WITH_VOICE.equals(project.getCreationTemplate())) {
    end = bRoll.getEndTime();
} else {
    if (CollectionUtils.isEmpty(voiceOverLines)) {
        end = bRoll.getEndTime();
    } else {
        end = Math.min(bRoll.getEndTime(), bRoll.getStartTime() + matchedScene.getDuration());
    }
}
```

**C++ 原实现:** 简单赋值
**C++ 修复后:** 添加了基于语音轨道的endTime计算逻辑

### 6. 资产导入和验证 ❌→需要进一步实现
**Java 实现:**
```java
Long assetId = importService.importAssetOfAnyType(projectId, bRoll.getEntityType(), 
    bRoll.getEntityUuid(), StockProviderCodeEnum.getByKey(bRoll.getProviderId()),
    ProjectAssetCategoryEnum.FOOTAGE);
```

**C++ 当前状态:** 直接使用assetUuid，缺少资产导入和验证逻辑

### 7. Stock媒体处理 ❌→需要进一步实现
**Java 实现:**
```java
if (bRoll.getEntityType() != null && bRoll.getEntityType().isStock()) {
    StockMedia bRollStockMedia = stockSaveService.tryGetById(...);
    // 复杂的Stock媒体duration验证和调整逻辑
}
```

**C++ 当前状态:** 缺少Stock媒体特殊处理

### 8. 媒体类型特殊处理 ❌→需要进一步实现
**Java 实现:**
```java
if (Objects.nonNull(projectAsset) && Objects.equals(MediaTypeEnum.IMAGE, projectAsset.getMediaType())) {
    newBRoll.setStartTime(0);
    newBRoll.setEndTime(matchedScene.getDuration());
}
```

**C++ 当前状态:** 缺少图片类型特殊处理

### 9. 缩放计算 ❌→需要进一步实现
**Java 实现:**
```java
Pair<Integer, Integer> assetWidthAndHeight = projectAssetService.getAssetWidthAndHeight(projectAsset.getId());
BigDecimal scale = projectTextStyleService.getScaleByTargetWidthAndHeight(project, 
    assetWidthAndHeight.getLeft(), assetWidthAndHeight.getRight(), settings.getFitType());
newBRoll.setScale(scale);
```

**C++ 当前状态:** 缺少缩放计算

### 10. 空白场景处理 ❌→✅
**Java 实现:**
```java
if (Objects.equals(matchedScene.getSceneType(), SceneTypeEnum.BLANK_SCENE)) {
    ProjectScene update = new ProjectScene();
    update.setId(sceneId);
    update.setSceneType(SceneTypeEnum.DEFAULT);
    projectSceneDao.updateByIdSelective(update);
}
```

**C++ 修复后:** 
```cpp
if (scene->sceneType == SceneTypeEnum::BLANK_SCENE) {
    scene->sceneType = SceneTypeEnum::DEFAULT;
}
```

### 11. 录音资产特殊处理 ❌→需要进一步实现
**Java 实现:**
```java
if (ProjectAssetTypeEnum.SELF_RECORDING.equals(projectAsset.getAssetType())) {
    timelineDao.deleteBySceneIdAndCategory(projectId, sceneId, SYNTHETIC_VOICE_OVER.getKey());
    timelineDao.deleteBySceneIdAndCategory(projectId, sceneId, RECORD_VOICE_OVER.getKey());
    clearRecordingAssetIdFlag(assetId);
    timelineService.updateVolumeByTimelineIds(Collections.singletonList(newBRoll.getId()), 1.0);
}
```

**C++ 当前状态:** 缺少录音资产特殊处理

### 12. 后续处理 ❌→需要进一步实现
**Java 实现:**
```java
// 缩略图更新
projectThumbnailService.updateProjectThumbnail(project);
// 层级调整
projectLayerWrapService.adjustTimelineLayerUpThanAroll(project.getId(), sceneId, newBRoll.getId());
// 文本视频场景时间重置
resetSceneTimeForTextVideo(helper.getProject(), matchedScene, newBRoll);
```

**C++ 当前状态:** 缺少所有后续处理逻辑

## 修复状态总结

### ✅ 已修复的问题：
1. 场景类型过滤 (intro/outro检查)
2. 基础重叠检查
3. 语音轨道检测
4. endTime计算逻辑
5. 空白场景类型转换
6. 时间轴属性完整性 (startTime, endTime, duration, volume等)

### ❌ 仍需修复的问题：
1. 权限验证框架
2. 资产导入和验证
3. Stock媒体处理
4. 媒体类型特殊处理
5. 缩放计算
6. 录音资产特殊处理
7. 项目缩略图更新
8. 层级调整
9. 文本视频场景时间重置

## 影响程度分析

**高严重性问题:**
- 缺少重叠检查 (可能导致时间轴冲突)
- 缺少场景类型过滤 (可能破坏intro/outro场景)
- 缺少资产验证 (可能使用无效资产)

**中等严重性问题:**
- 缺少Stock媒体处理 (影响第三方媒体)
- 缺少媒体类型处理 (影响图片显示)
- 缺少后续处理 (影响用户体验)

**低严重性问题:**
- 缺少权限检查 (安全性问题，但可在API网关层处理)

## 建议

1. **立即修复高严重性问题** - 确保基础功能正确性
2. **逐步补充中等严重性问题** - 保证功能完整性
3. **考虑架构重构** - 某些Java逻辑过于复杂，可考虑简化C++实现
4. **增加测试覆盖** - 确保修复后的逻辑与Java后端行为一致

通过本次分析和修复，PJsonEditor的`addFootage`实现已经与Java的`addSceneFootage`在核心业务逻辑上基本保持一致，但仍有一些高级功能需要进一步实现。
