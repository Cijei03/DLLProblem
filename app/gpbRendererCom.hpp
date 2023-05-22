#pragma once
#include "gpbGlobalDefs.hpp"
#include <memory>
#include <variant>

struct gpbRMTextureInfo
{
	std::unique_ptr<TGMath::ui16vec2[]> mipmapDimensions;
	std::unique_ptr<void*[]> mipmapsData;
	std::unique_ptr<gpbUINT32[]> mipmapsSizes;
	gpbUINT32 mipmapsCount;
};
template<typename T>
struct gpbRMGeometryEntry
{
	std::unique_ptr<T[]> data;
	gpbUINT32 dataSize;
};
struct gpbRMGeometryInfo
{
	gpbUINT32 verticesCount;
	gpbRMGeometryEntry<gpbVECTOR3> positions;
};

using gpbRMResourceName = std::variant<gpbUINT32, void*>;