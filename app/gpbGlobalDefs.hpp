#pragma once

#include <cstdint>

using gpbUINT8 = uint8_t;
using gpbUINT16 = uint16_t;
using gpbUINT32 = uint32_t;
using gpbUINT64 = uint64_t;

using gpbINT8 = int8_t;
using gpbINT16 = int16_t;
using gpbINT32 = int32_t;
using gpbINT64 = int64_t;

using gpbFLOAT32 = float;
using gpbFLOAT64 = double;

using gpbWindowPtr = void*;

#include <TGMath/TGMath.hpp>

using gpbVECTOR2 = TGMath::f32vec2;
using gpbVECTOR3 = TGMath::f32vec3;
using gpbVECTOR4 = TGMath::f32vec4;

using gpbMATRIX2 = TGMath::f32mat2;
using gpbMATRIX3 = TGMath::f32mat3;
using gpbMATRIX4 = TGMath::f32mat4;

using gpbRegisterEntry = gpbUINT32;