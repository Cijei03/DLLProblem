#include "../include/gpbRendererManager.hpp"
#include "../include/gpbExceptions.hpp"
#include "../include/gpbRendererFunctionsTable.hpp"
#include "../ImporterDDS/include/gpbImporterDDS.hpp"
#include "../include/gpbRendererCom.hpp"
#include "../ModelBuilder/MBLib/include/mbImporter.hpp"
#include <filesystem>

void gpbRMM::setRenderTargetViewClearColor(float r, float g, float b, float a)
{
	gpbRMSetRenderTargetViewClearColor(r, g, b, a);
}
void gpbRMM::clearRenderTargetView(gpbRenderTargetViewType type)
{
	gpbRMClearRenderTargetView(type);
}
void gpbRMM::present()
{
	gpbRMPresent();
}
gpbRMResourceName gpbRMM::loadTexture(const std::string& fileName)
{
	if (!std::filesystem::exists(fileName))
		throw gpbResourceNotFound(fileName);
	
	gpbImporterDDS importer(fileName);

	gpbRMTextureInfo info{};
	info.mipmapsCount = importer.getMipsCount();
	info.mipmapDimensions = std::make_unique<TGMath::ui16vec2[]>(info.mipmapsCount);
	info.mipmapsData = std::make_unique<void* []>(info.mipmapsCount);
	info.mipmapsSizes = std::make_unique<gpbUINT32[]>(info.mipmapsCount);

	for (gpbUINT32 i = 0; i < info.mipmapsCount; i++)
	{
		auto cache = importer.getMipInfo(i);
		info.mipmapDimensions[i] = TGMath::ui16vec2(cache.width, cache.height);
		info.mipmapsData[i] = importer.getPixelDataPtr(i);
		info.mipmapsSizes[i] = cache.size;
	}
	
	return gpbRMCreateTexture(info);
}
void gpbRMM::deleteTexture(gpbRMResourceName textureName)
{
	gpbRMDeleteTexture(textureName);
}

gpbRMResourceName gpbRMM::createShaderPipeline(std::string vertexShaderFileName, std::string pixelShaderFileName)
{
	vertexShaderFileName = "shaders/" + vertexShaderFileName;
	pixelShaderFileName = "shaders/" + pixelShaderFileName;
	if (gpbCMM::configBuffer.API == gpbClientAPI::OPENGL)
	{
		vertexShaderFileName += ".vert";
		pixelShaderFileName += ".frag";
	}
	else if (gpbCMM::configBuffer.API == gpbClientAPI::D3D11)
	{
		
	}
	else if (gpbCMM::configBuffer.API == gpbClientAPI::VULKAN)
	{
		vertexShaderFileName += ".spv.vert";
		pixelShaderFileName += ".spv.frag";
	}
	else if (gpbCMM::configBuffer.API == gpbClientAPI::D3D12)
	{
		
	}

	if (!std::filesystem::exists(vertexShaderFileName))
		throw gpbResourceNotFound(vertexShaderFileName);
	else if (!std::filesystem::exists(pixelShaderFileName))
		throw gpbResourceNotFound(pixelShaderFileName);

	return gpbRMCreateShaderPipeline(&vertexShaderFileName[0], &pixelShaderFileName[0]);
}
void gpbRMM::deleteShaderPipeline(gpbRMResourceName internalName)
{
	gpbRMDeletePipeline(internalName);
}
#include <iostream>
gpbRMResourceName gpbRMM::loadModel(const std::string& fileName)
{
	if (!std::filesystem::exists(fileName))
	{
		throw gpbResourceNotFound(fileName);
	}

	mbImporter importer;
	importer.loadMesh(fileName);

	
	
	gpbRMGeometryInfo info{};
	info.verticesCount = importer.getVerticesCount();
	
	info.positions.dataSize = importer.getBufferSize(mbImporterMask::VERTEX_POSITION).value();
	
	{
		auto cache = importer.getData(mbImporterMask::VERTEX_POSITION).value();
		info.positions.data = std::make_unique<gpbVECTOR3[]>(info.verticesCount);
		std::memcpy(info.positions.data.get(), cache.get(), sizeof(gpbVECTOR3) * info.verticesCount);
	}
	return gpbRMCreateModel(info);
}

#if _WIN64
#define OGL_LIB_NAME "rmOpenGL.dll"
#define D3D11_LIB_NAME "rmD3D11.dll"
#define VULKAN_LIB_NAME "rmVulkan.dll"
#define D3D12_LIB_NAME "rmD3D12.dll"

#include <Windows.h>

static HMODULE gpbRendererHandle{};

template <typename T>
static void loadSymbol(T& destination, std::string symbolName)
{
	destination = reinterpret_cast<T>(GetProcAddress(gpbRendererHandle, &symbolName[0]));
	if (destination == nullptr)
	{
		symbolName = "[GPB RM] Couldn't open renderer with: " + symbolName + '\n';
		OutputDebugStringA(&symbolName[0]);
	}
}
#include <iostream>
void gpbRMM::open(gpbClientAPI api, gpbWindowPtr window, gpbVECTOR2 windowDimensions)
{
	// Try open external module.
	if (api == gpbClientAPI::OPENGL)
	{
		gpbRendererHandle = LoadLibraryA(OGL_LIB_NAME);
		if (!gpbRendererHandle)
			throw gpbOpenGLUnsupported();
	}		
	else if (api == gpbClientAPI::D3D11)
	{
		gpbRendererHandle = LoadLibraryA(D3D11_LIB_NAME);
		if (!gpbRendererHandle)
			throw gpbD3D11Unsupported();
	}
	else if (api == gpbClientAPI::VULKAN)
	{
		gpbRendererHandle = LoadLibraryA(VULKAN_LIB_NAME);
		if (!gpbRendererHandle)
			throw gpbVulkanUnsupported();
	}
	else if (api == gpbClientAPI::D3D12)
	{
		gpbRendererHandle = LoadLibraryA(D3D12_LIB_NAME);
		if (!gpbRendererHandle)
			throw gpbD3D12Unsupported();
	}

	// If succeed load symbols from module.
	loadSymbol(gpbRMOpen, "open");
	loadSymbol(gpbRMSetRenderTargetViewClearColor, "setRenderTargetViewClearColor");
	loadSymbol(gpbRMClearRenderTargetView, "clearRenderTargetView");
	loadSymbol(gpbRMPresent, "present");
	loadSymbol(gpbRMCreateTexture, "createTexture");
	loadSymbol(gpbRMDeleteTexture, "deleteTexture");
	loadSymbol(gpbRMCreateShaderPipeline, "createShaderPipeline");
	loadSymbol(gpbRMDeletePipeline, "deleteShaderPipeline");	
	loadSymbol(gpbRMRenderUIElement, "renderUIElement");
	loadSymbol(gpbRMUpdateDisplayDimensions, "updateDisplayDimensions");
	loadSymbol(gpbRMOpenFont, "openFont");
	loadSymbol(gpbRMRenderText, "renderText");
	loadSymbol(gpbRMSetFontType, "setFontType");
	loadSymbol(gpbRMSetTextPos, "setTextPos");
	loadSymbol(gpbRMSetTextColor, "setTextColor");
	loadSymbol(gpbRMSetTextSize, "setTextSize");
	loadSymbol(gpbRMSetTextDepth, "setTextDepth");
	loadSymbol(gpbRMSetTextJustification, "setTextJustification");
	loadSymbol(gpbRMSetViewport, "setViewport");
	loadSymbol(gpbRMRegisterUIElement, "registerUIElement");
	loadSymbol(gpbRMRenderUIRegister, "renderUIRegister");
	loadSymbol(gpbRMGetPointedEntry, "getPointedEntry");
	loadSymbol(gpbRMCreateModel, "createModel");
	std::cout << "Opening..." << std::endl;

	// Open module itself.
	gpbRMOpen(window);
	std::cout << "Opened." << std::endl;
	gpbRMUpdateDisplayDimensions(windowDimensions);
	std::cout << "Setting viewport..." << std::endl;
	gpbRMSetViewport(0, 0, windowDimensions.x, windowDimensions.y);
}

void gpbRMM::close()
{
	// Close linkage to external module.
	FreeLibrary(gpbRendererHandle);

	// Free functions pointers.
	gpbRMOpen = nullptr;
	gpbRMClearRenderTargetView = nullptr;
	gpbRMPresent = nullptr;
	gpbRMSetRenderTargetViewClearColor = nullptr;
	gpbRMCreateTexture = nullptr;
	gpbRMDeleteTexture = nullptr;
	gpbRMCreateShaderPipeline = nullptr;
	gpbRMDeletePipeline = nullptr;
	gpbRMRenderUIElement = nullptr;
	gpbRMUpdateDisplayDimensions = nullptr;
	gpbRMOpenFont = nullptr;
	gpbRMRenderText = nullptr;
	gpbRMSetFontType = nullptr;
	gpbRMSetTextColor = nullptr;
	gpbRMSetTextSize = nullptr;
	gpbRMSetTextDepth = nullptr;
	gpbRMSetTextJustification = nullptr;
	gpbRMSetViewport = nullptr;
	gpbRMRegisterUIElement = nullptr;
	gpbRMRenderUIRegister = nullptr;
	gpbRMGetPointedEntry = nullptr;
	gpbRMCreateModel = nullptr;
}
#elif __linux__
#endif
void gpbRMM::updateDisplayDimensions(gpbVECTOR2 displayDimensions)
{
	gpbRMUpdateDisplayDimensions(displayDimensions);
}
void gpbUIRM::renderUIElement(gpbRMResourceName textureName, gpbVECTOR3 posOffset, gpbFLOAT32 size, const gpbVECTOR4& overridingColor, gpbRMResourceName shadersPipelineName, gpbUINT32 flipped)
{
	gpbRMRenderUIElement(textureName, posOffset, size, overridingColor, shadersPipelineName, flipped);
}
gpbRMResourceName gpbRMM::openFont(const std::string& fileName, gpbRMResourceName textureName)
{
	if (!std::filesystem::exists(fileName))
		throw gpbResourceNotFound(fileName);

	return gpbRMOpenFont(&fileName[0], textureName);
}
void gpbUIRM::renderText(const std::string& text, gpbRMResourceName shadersPipelineName)
{
	gpbRMRenderText(&text[0], shadersPipelineName);
}
void gpbUIRM::setFontType(gpbRMResourceName fontName)
{
	gpbRMSetFontType(fontName);
}
void gpbUIRM::setTextPos(gpbVECTOR2 pos)
{
	gpbRMSetTextPos(pos);
}
void gpbUIRM::setTextColor(const gpbVECTOR4& color)
{
	gpbRMSetTextColor(color);
}
void gpbUIRM::setTextSize(gpbFLOAT32 size)
{
	gpbRMSetTextSize(size);
}
void gpbUIRM::setTextDepth(gpbFLOAT32 size)
{
	gpbRMSetTextDepth(size);
}
void gpbUIRM::setTextJustification(bool justification)
{
	gpbRMSetTextJustification(justification);
}
void gpbRMM::setViewport(gpbFLOAT32 x, gpbFLOAT32 y, gpbFLOAT32 width, gpbFLOAT32 height)
{
	gpbRMSetViewport(x, y, width, height);
}
gpbRegisterEntry gpbUIRM::registerUIElement()
{
	return gpbRMRegisterUIElement();
}
void gpbUIRM::renderUIRegister(gpbVECTOR3 posOffset, gpbFLOAT32 size, gpbRMResourceName shadersPieplineName, gpbRMResourceName textureName, gpbRegisterEntry regEntry)
{
	gpbRMRenderUIRegister(posOffset, size, shadersPieplineName, textureName, regEntry);
}
gpbRegisterEntry gpbUIRM::getPointedEntry(gpbVECTOR2 cursorPos)
{
	return gpbRMGetPointedEntry(cursorPos);
}