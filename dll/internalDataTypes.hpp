#pragma once
#include "../../include/gpbRendererCom.hpp"
#include "../../include/gpbFontInfo.hpp"

struct Texture2DLayout
{
	gpbUINT32 infoBuffer{};
	gpbUINT32 texture{};
};

struct FontLayout
{
	gpbRMResourceName fontName{};
	gpbRMResourceName textureName{};
	std::shared_ptr<gpbFLOAT32[]> charactersInfo{};
};

struct CharacterInfo
{
	gpbVECTOR2 posOffset{};
	gpbUINT32 symbolID{};
};

struct TextRenderingInfo
{
	gpbFLOAT32 size{};
	gpbFLOAT32 depth{};
	gpbFLOAT32 aspectRatio{};
	gpbUINT32 justify{};
	gpbVECTOR4 color{};
	gpbVECTOR2 posOffset{};
};

struct GeometryInfo
{
	gpbRMResourceName VAO;
	gpbUINT32 verticesCount;

	gpbUINT32 posBuffer;
};