#pragma once

#include <iostream>
#include <map>
#include <variant>

#include <Util/Log.h>
#include <EngineAPI/EngineAPI.h>
#include <UniUtil/Time.h>
#include <Util/ConfigConst.h>
#include <Util/Reader.h>
#include <Util/RJson.h>

#include <glm/vec2.hpp>

#include <BComp/Composite.h>
#include <BComp/FootageImage.h>
#include <BLayer/LayerComp.h>
#include <BLayer/LayerImage.h>
#include <BLayer/LayerVideo.h>
#include <BMaterial/Effect.h>
#include <BMaterial/Mask.h>
#include <BProject/Project.h>
#include <BPlayer/Player.h>
#include <BExporter/Exporter.h>

#include <BAPIHelper/BusinessAPIHelper.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>

const std::string PathPrefix = "/storage/emulated/0/Android/data/com.am.surfaceview_manual/files";
const std::string InternalResDir = "/Res/";
const std::string LogName = "Demo1";
const std::string LogBase = PathPrefix + "/Logs";
const std::string LogInfo = LogBase + "/LogInfo.";
const std::string LogWarning = LogBase + "/LogWarning.";
const std::string LogErr = LogBase + "/LogErr.";
const std::string LogFatal = LogBase + "/LogFatal.";

int64_t CreatePropInfoFromJsonPropInfo(UniUtil::ValType valType, const rj::Value& propJson, UniUtil::PropInfo& propInfo);

int64_t CreatePropInfoFromJsonPropHandleArray(UniUtil::TypeInfo typeInfo, const rj::Value& jsonPropHandleArray, UniUtil::PropInfo& propInfo);

class IcebergRender
{
public:
	int64_t Init(EGLDisplay display, EGLSurface surface, EGLContext context, ANativeWindow* nativeWindow, uint32_t width, uint32_t height);

	int64_t RenderLoop();

private:
	bool isInit = false;

	BProj::Project proj;

	BPlayer::Sp_Player player;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	ANativeWindow* nativeWindow;
    uint32_t width;
    uint32_t height;

};