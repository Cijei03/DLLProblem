#pragma once

#ifndef DLL_EXPORT
//#define DLL_EXPORT __declspec(dllexport)
#define DLL_EXPORT
#endif

#include "../../include/gpbGlobalDefs.hpp"
#include "../../include/gpbRendererManager.hpp"
#include "../../include/gpbRendererCom.hpp"
#include "../../include/gpbClientCamera.hpp"
#include "../../include/gpbStaticActor.hpp"


//extern "C++"
//{
	/* BASIC RESOURCES OPERATIONS */
	DLL_EXPORT void open(gpbWindowPtr window);
	DLL_EXPORT void close();

	DLL_EXPORT void setRenderTargetViewClearColor(gpbFLOAT32 r, gpbFLOAT32 g, gpbFLOAT32 b, gpbFLOAT32 a);
	DLL_EXPORT void clearRenderTargetView(gpbRenderTargetViewType type);
	DLL_EXPORT void present();
	DLL_EXPORT void setViewport(gpbFLOAT32 x, gpbFLOAT32 y, gpbFLOAT32 width, gpbFLOAT32 height);
	
	DLL_EXPORT gpbRMResourceName createTexture(const gpbRMTextureInfo& info);
	DLL_EXPORT void deleteTexture(gpbRMResourceName internalName);

	DLL_EXPORT gpbRMResourceName createShaderPipeline(const char* vertexShaderFileName, const char* pixelShaderFileName);
	DLL_EXPORT void deleteShaderPipeline(gpbRMResourceName internalName);

	DLL_EXPORT void updateDisplayDimensions(gpbVECTOR2 dimensions);
	
	/* TEXT RENDERING */
	DLL_EXPORT gpbRMResourceName openFont(const char* fileName, gpbRMResourceName textureName);
	DLL_EXPORT void setFontType(gpbRMResourceName fontName);
	DLL_EXPORT void setTextPos(gpbVECTOR2 posOffset);
	DLL_EXPORT void setTextColor(const gpbVECTOR4& color);
	DLL_EXPORT void setTextSize(gpbFLOAT32 size);
	DLL_EXPORT void setTextDepth(gpbFLOAT32 depth);
	DLL_EXPORT void setTextJustification(bool justification);
	DLL_EXPORT void renderText(const char* text, gpbRMResourceName shadersPipelineName);

	/* UI */
	DLL_EXPORT void renderUIElement(gpbRMResourceName textureName, gpbVECTOR3 posOffset, gpbFLOAT32 size, const gpbVECTOR4& overridingColor, gpbRMResourceName shadersPipelineName, gpbUINT32 flipped);
	DLL_EXPORT gpbRegisterEntry registerUIElement();
	DLL_EXPORT void renderUIRegister(gpbVECTOR3 posOffset, gpbFLOAT32 size, gpbRMResourceName shadersPieplineName, gpbRMResourceName textureName, gpbRegisterEntry regEntry);
	DLL_EXPORT gpbRegisterEntry getPointedEntry(gpbVECTOR2 cursorPos);

	/* 3D RESOURCES */
	DLL_EXPORT gpbRMResourceName createModel(const gpbRMGeometryInfo& info);
	DLL_EXPORT void allocateStaticActor(gpbStaticActor& actor);
	DLL_EXPORT void setCamera(const gpbClientCamera& camera);
	
//}