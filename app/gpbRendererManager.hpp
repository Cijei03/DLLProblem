#pragma once
#include "gpbGlobalDefs.hpp"
#include "gpbConfigManager.hpp"
#include "gpbRendererCom.hpp"
#include <string>

enum class gpbRenderTargetViewType
{
	MAIN,
	SELECTION
};

// GPB Renderer Module Manager
class gpbRMM
{
public:
	static void open(gpbClientAPI api, gpbWindowPtr window, gpbVECTOR2 displayDimensions);
	static void close();

	static void setRenderTargetViewClearColor(float r, float g, float b, float a);
	static void clearRenderTargetView(gpbRenderTargetViewType type);
	static void present();

	static gpbRMResourceName loadTexture(const std::string& fileName);
	static void deleteTexture(gpbRMResourceName textureName);

	static gpbRMResourceName createShaderPipeline(std::string vertexShaderFileName, std::string pixelShaderFileName);
	static void deleteShaderPipeline(gpbRMResourceName internalName);

	static void updateDisplayDimensions(gpbVECTOR2 displayDimensions);
	
	static gpbRMResourceName openFont(const std::string& fileName, gpbRMResourceName textureName);
	static void setViewport(gpbFLOAT32 x, gpbFLOAT32 y, gpbFLOAT32 width, gpbFLOAT32 height);

	static gpbRMResourceName loadModel(const std::string& fileName);
};

// GPB User Interface Renderer Module
class gpbUIRM
{
public:
	static void setFontType(gpbRMResourceName fontName);
	static void setTextPos(gpbVECTOR2 posOffset);
	static void setTextColor(const gpbVECTOR4& color);
	static void setTextSize(gpbFLOAT32 size);
	static void setTextDepth(gpbFLOAT32 depth);
	static void setTextJustification(bool state);
	static void renderText(const std::string& text, gpbRMResourceName shadersPipelineName);

	static void renderUIElement(gpbRMResourceName textureName, gpbVECTOR3 posOffset, gpbFLOAT32 size, const gpbVECTOR4& overridingColor, gpbRMResourceName shadersPipelineName, gpbUINT32 flipped);

	static gpbRegisterEntry registerUIElement();
	static void renderUIRegister(gpbVECTOR3 posOffset, gpbFLOAT32 size, gpbRMResourceName shadersPieplineName, gpbRMResourceName textureName, gpbRegisterEntry regEntry);
	static gpbRegisterEntry getPointedEntry(gpbVECTOR2 cursorPos);
};