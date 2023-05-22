#pragma once
#include "gpbGlobalDefs.hpp"
#include "../include/gpbRendererCom.hpp"

// RM - Rendering Module
// *Func* - Opaque type for functions pointers.

using gpbFuncOpen = void(*)(gpbWindowPtr);
static gpbFuncOpen gpbRMOpen{};

using gpbFuncSetRenderTargetViewClearColor = void(*)(float, float, float, float);
static gpbFuncSetRenderTargetViewClearColor gpbRMSetRenderTargetViewClearColor{};

using gpbFuncClearRenderTargetView = void(*)(gpbRenderTargetViewType);
static gpbFuncClearRenderTargetView gpbRMClearRenderTargetView{};

using gpbFuncPresent = void(*)();
static gpbFuncPresent gpbRMPresent{};

using gpbFuncLoadTexture = gpbRMResourceName(*)(const gpbRMTextureInfo& info);
static gpbFuncLoadTexture gpbRMCreateTexture{};

using gpbFuncDeleteTexture = void(*)(gpbRMResourceName);
static gpbFuncDeleteTexture gpbRMDeleteTexture{};

using gpbFuncCreateShaderPipeline = gpbRMResourceName(*)(const char*, const char*);
static gpbFuncCreateShaderPipeline gpbRMCreateShaderPipeline{};

using gpbFuncDeleteShaderPipeline = void(*)(gpbRMResourceName);
static gpbFuncDeleteShaderPipeline gpbRMDeletePipeline{};

using gpbFuncRenderUIElement = void(*)(gpbRMResourceName, gpbVECTOR3, gpbFLOAT32, const gpbVECTOR4&, gpbRMResourceName, gpbUINT32);
static gpbFuncRenderUIElement gpbRMRenderUIElement{};

using gpbFuncUpdateDisplayDimensions = void(*)(gpbVECTOR2);
static gpbFuncUpdateDisplayDimensions gpbRMUpdateDisplayDimensions{};

using gpbFuncOpenFont = gpbRMResourceName(*)(const char*, gpbRMResourceName);
static gpbFuncOpenFont gpbRMOpenFont{};

using gpbFuncRenderText = void (*)(const char*, gpbRMResourceName);
static gpbFuncRenderText gpbRMRenderText{};

using gpbFuncSetFontType = void (*)(gpbRMResourceName);
static gpbFuncSetFontType gpbRMSetFontType{};

using gpbFuncSetTextPos = void(*)(gpbVECTOR2);
static gpbFuncSetTextPos gpbRMSetTextPos{};

using gpbFuncSetTextColor = void(*)(const gpbVECTOR4&);
static gpbFuncSetTextColor gpbRMSetTextColor{};

using gpbFuncSetTextSize = void(*)(gpbFLOAT32);
static gpbFuncSetTextSize gpbRMSetTextSize{};

using gpbFuncSetTextDepth = void(*)(gpbFLOAT32);
static gpbFuncSetTextDepth gpbRMSetTextDepth{};

using gpbFuncSetTextJustification = void(*)(bool);
static gpbFuncSetTextJustification gpbRMSetTextJustification{};

using gpbFuncSetViewport = void(*)(gpbFLOAT32, gpbFLOAT32, gpbFLOAT32, gpbFLOAT32);
static gpbFuncSetViewport gpbRMSetViewport{};

using gpbFuncRegisterUIElement = gpbRegisterEntry(*)();
static gpbFuncRegisterUIElement gpbRMRegisterUIElement{};

using gpbFuncRenderUIRegister = void (*)(gpbVECTOR3, gpbFLOAT32, gpbRMResourceName, gpbRMResourceName, gpbRegisterEntry);
static gpbFuncRenderUIRegister gpbRMRenderUIRegister{};

using gpbFuncGetPointerEntry = gpbRegisterEntry(*)(gpbVECTOR2);
static gpbFuncGetPointerEntry gpbRMGetPointedEntry{};

using gpbFuncCreateModel = gpbRMResourceName(*)(const gpbRMGeometryInfo&);
static gpbFuncCreateModel gpbRMCreateModel{};