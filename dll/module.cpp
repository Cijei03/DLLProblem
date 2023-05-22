#include "../include/module.hpp"
#include "../../include/gpbExceptions.hpp"
#include "../include/internalDataTypes.hpp"
#include "../include/API.hpp"
#include <vector>
#include <fstream>
#include <string>
#include <memory>

static TextRenderingInfo textRenderingInfo{};
void setViewport(gpbFLOAT32 x, gpbFLOAT32 y, gpbFLOAT32 width, gpbFLOAT32 height)
{
	glViewport(x, y, width, height);
	textRenderingInfo.aspectRatio = width / height;
}
static gpbUINT32 UIElementVAO{};
static gpbUINT32 UIElementRenderInfoBuffer{};
static gpbUINT32 displayInfoBuffer{};
static gpbUINT32 fontCoordsBuffer{};
static gpbUINT32 characterInfoBuffer{};
static gpbUINT32 textRenderingInfoBuffer{};
static gpbUINT32 selectionRegisterEntryBuffer{};

static gpbUINT32 selectionFramebuffer{};
static gpbUINT32 selectionTexture{};
static gpbUINT32 selectionDepthBuffer{};

static gpbUINT32 cameraBuffer{};
void present()
{
	swap();
}
void open(gpbWindowPtr window)
{
	std::cout << "OPENING CONTEXT..." << std::endl;
	openContext(window);
	std::cout << "QUERYING EXTENSIONS..." << std::endl;
	queryExtensions();	
	std::cout << "ALLOCATING REQUIRED RESOURCES..." << std::endl;

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	// Enable blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup basic resources.
	glCreateVertexArrays(1, &UIElementVAO);
	{
		// Setup display info buffer.
		glCreateBuffers(1, &displayInfoBuffer);
		glNamedBufferStorage(displayInfoBuffer, 16, nullptr, GL_MAP_WRITE_BIT);

		// Setup UI element render info.
		glCreateBuffers(1, &UIElementRenderInfoBuffer);
		glNamedBufferStorage(UIElementRenderInfoBuffer, 48, nullptr, GL_MAP_WRITE_BIT);

		// Setup basic font buffers.
		glCreateBuffers(1, &fontCoordsBuffer);
		{
			std::unique_ptr<gpbVECTOR2[]> coords = std::make_unique<gpbVECTOR2[]>(256);
			{
				constexpr float coordPerSymbol = 0.0625f;
				for (gpbUINT32 row = 0; row < 16; row++)
				{
					for (gpbUINT32 column = 0; column < 16; column++)
					{
						coords[row * 16 + column] = gpbVECTOR2(column * coordPerSymbol, row * coordPerSymbol);
					}
				}
			}
			glNamedBufferStorage(fontCoordsBuffer, sizeof(gpbVECTOR2) * 256, coords.get(), NULL);
		}
		glCreateBuffers(1, &characterInfoBuffer);
		glNamedBufferStorage(characterInfoBuffer, 16, nullptr, GL_MAP_WRITE_BIT); 

		glCreateBuffers(1, &textRenderingInfoBuffer);
		glNamedBufferStorage(textRenderingInfoBuffer, 48, nullptr, GL_MAP_WRITE_BIT);

		// Setup UI selection buffer.
		glCreateBuffers(1, &selectionRegisterEntryBuffer);
		glNamedBufferStorage(selectionRegisterEntryBuffer, 16, nullptr, GL_MAP_WRITE_BIT);

		// Create camera buffer.
		glCreateBuffers(1, &cameraBuffer);
		glNamedBufferStorage(cameraBuffer, 64, nullptr, GL_MAP_WRITE_BIT);
	}
}
void setRenderTargetViewClearColor(gpbFLOAT32 r, gpbFLOAT32 g, gpbFLOAT32 b, gpbFLOAT32 a)
{
	glClearColor(r, g, b, a);
}
void clearRenderTargetView(gpbRenderTargetViewType type)
{
	if (type == gpbRenderTargetViewType::MAIN)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	else if (type == gpbRenderTargetViewType::SELECTION)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, selectionFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
static std::vector<std::shared_ptr<Texture2DLayout>> Textures;
//static std::vector<Texture2DLayout> Textures;
static Texture2DLayout& getNamedTextureLayout(gpbRMResourceName name)
{
	return *reinterpret_cast<Texture2DLayout*>(std::get<void*>(name));
	//for (gpbUINT32 i = 0; i < Textures.size(); i++)
	{
		//if (Textures[i].name == name)
			//return Textures[i];
	}
}
gpbRMResourceName createTexture(const gpbRMTextureInfo& info)
{
	// Declare cache.
	auto cache = std::make_shared<Texture2DLayout>();
	//Texture2DLayout cache{};

	// Create texture info buffer.
	glCreateBuffers(1, &cache.get()->infoBuffer);
	{
		gpbFLOAT32 textureAspect[4]
		{
			gpbFLOAT32(info.mipmapDimensions[0].x) / gpbFLOAT32(info.mipmapDimensions[0].y)
		};
		glNamedBufferStorage(cache.get()->infoBuffer, 16, textureAspect, NULL);
	}

	// Create texture itself.
	{
		gpbUINT32& texture = cache.get()->texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, info.mipmapsCount, GL_COMPRESSED_RGBA_BPTC_UNORM, info.mipmapDimensions[0].x, info.mipmapDimensions[0].y);
		for (gpbUINT32 i = 0; i < info.mipmapsCount; i++)
		{
			glCompressedTextureSubImage2D(texture, i, 0, 0, info.mipmapDimensions[i].x, info.mipmapDimensions[i].y, GL_COMPRESSED_RGBA_BPTC_UNORM, info.mipmapsSizes[i], info.mipmapsData[i]);
		}
		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//cache.name.emplace<gpbUINT32>(name);
	}

	Textures.push_back(cache);

	return gpbRMResourceName(reinterpret_cast<void*>(cache.get()));
}
void deleteTexture(gpbRMResourceName internalName)
{
	// Cast to proper pointer.
	auto texture = reinterpret_cast<Texture2DLayout*>(std::get<void*>(internalName));
	// Free GPU resources.
	glDeleteTextures(1, &texture->texture);
	glDeleteBuffers(1, &texture->infoBuffer);
	// Free CPU resources.
	for (gpbUINT32 i = 0; i < Textures.size(); i++)
	{
		if (Textures[i].get() == texture)
		{
			Textures.erase(Textures.begin() + i);
			break;
		}
	}
	/*
	for (gpbUINT32 i = 0; i < Textures.size(); i++)
	{
		if (Textures[i].name == internalName)
		{
			glDeleteTextures(1, &std::get<gpbUINT32>(Textures[i].name));
			Textures.erase(Textures.begin() + i);
			break;
		}
	}*/
}
static std::vector<gpbRMResourceName> ShaderPipelines;
gpbRMResourceName createShaderPipeline(const char* vertexShaderFileName, const char* pixelShaderFileName)
{
	auto loadShaderSource = [](std::string& source, const std::string& fileName) -> void
	{
		std::ifstream file(fileName);
		std::getline(file, source, '\0');
		file.close();
	};

	std::string vsSource{}, psSource{};
	// Load shaders source codes.
	{
		loadShaderSource(vsSource, vertexShaderFileName);
		loadShaderSource(psSource, pixelShaderFileName);
	}

	// Create and compile shaders.
	gpbUINT32 vs{}, ps{};
	{
		vs = glCreateShader(GL_VERTEX_SHADER);
		ps = glCreateShader(GL_FRAGMENT_SHADER);

		const char* vsCacheSource = &vsSource[0];
		const char* psCacheSource = &psSource[0];

		glShaderSource(vs, 1, &vsCacheSource, nullptr);
		glShaderSource(ps, 1, &psCacheSource, nullptr);

		glCompileShader(vs);
		glCompileShader(ps);
	}

	// Create and link to program.
	gpbRMResourceName cache{};
	{
		gpbUINT32 name = glCreateProgram();
		glAttachShader(name, vs);
		glAttachShader(name, ps);

		glLinkProgram(name);

		cache.emplace<gpbUINT32>(name);
	}

	// Clean up.
	{
		glDeleteShader(vs);
		glDeleteShader(ps);
	}

	ShaderPipelines.push_back(cache);
	if (!ShaderPipelines.size())
		throw;

	return cache;
}
void deleteShaderPipeline(gpbRMResourceName internalName)
{
	gpbUINT32& fromUserName = std::get<gpbUINT32>(internalName);
	for (gpbUINT32 i = 0; i < ShaderPipelines.size(); i++)
	{
		gpbUINT32& currentName = std::get<gpbUINT32>(ShaderPipelines[i]);
		if (currentName == fromUserName)
		{
			glDeleteProgram(currentName);
			ShaderPipelines.erase(ShaderPipelines.begin() + i);
			break;
		}
	}
}
void renderUIElement(gpbRMResourceName textureName, gpbVECTOR3 posOffset, gpbFLOAT32 size, const gpbVECTOR4& overridingColor, gpbRMResourceName shadersPipelineName, gpbUINT32 flipped)
{
	// Update UI Element Render Info
	{
		struct bufContent
		{
			gpbVECTOR3 posOffset;
			gpbFLOAT32 size;
			gpbUINT32 shouldFlip[4];
			gpbVECTOR4 overrideColor;
		};
		bufContent cache
		{
			posOffset,
			size,
			flipped, 0, 0, 0,
			overridingColor
		};
		auto mappedBuf = glMapNamedBuffer(UIElementRenderInfoBuffer, GL_WRITE_ONLY);
		std::memcpy(mappedBuf, &cache, sizeof(bufContent));
		glUnmapNamedBuffer(UIElementRenderInfoBuffer);
	}

	// Bind resources.
	glUseProgram(std::get<gpbUINT32>(shadersPipelineName));
	glBindTextureUnit(0, std::get<gpbUINT32>(textureName));
	glBindVertexArray(UIElementVAO);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, getNamedTextureLayout(textureName).infoBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, displayInfoBuffer);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, UIElementRenderInfoBuffer, 0, 32);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, UIElementRenderInfoBuffer, 32, 16);

	// Draw.
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
static gpbVECTOR2 displayDimensions{};
void updateDisplayDimensions(gpbVECTOR2 dimensions)
{
	std::cout << "UPDATING: " << dimensions.x << ' ' << dimensions.y << std::endl;
	displayDimensions = dimensions;

	// Compute aspect.
	dimensions.x = dimensions.x / dimensions.y;

	// Update buffer content.
	auto mappedBuf = glMapNamedBuffer(displayInfoBuffer, GL_WRITE_ONLY);
	std::memcpy(mappedBuf, &dimensions, 4);
	glUnmapNamedBuffer(displayInfoBuffer);

	// Update selection framebuffer.
	{
		// Delete if it already exists.
		if (selectionFramebuffer)
		{
			glDeleteFramebuffers(1, &selectionFramebuffer);
			glDeleteTextures(1, &selectionTexture);
			glDeleteRenderbuffers(1, &selectionDepthBuffer);
		}
		
		// Re-create.
		glCreateTextures(GL_TEXTURE_2D, 1, &selectionTexture);
		glTextureStorage2D(selectionTexture, 1, GL_R32UI, displayDimensions.x, displayDimensions.y);

		glCreateRenderbuffers(1, &selectionDepthBuffer);
		glNamedRenderbufferStorage(selectionDepthBuffer, GL_DEPTH_COMPONENT32, displayDimensions.x, displayDimensions.y);

		glCreateFramebuffers(1, &selectionFramebuffer);
		glNamedFramebufferTexture(selectionFramebuffer, GL_COLOR_ATTACHMENT0, selectionTexture, 0);
		glNamedFramebufferRenderbuffer(selectionFramebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, selectionDepthBuffer);
				
		// Check framebuffers has been properly created.
		if (glCheckNamedFramebufferStatus(selectionFramebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw gpbOpenGLUnsupported();
	}
}

gpbUINT32 getResourceName()
{
	static gpbUINT32 ID = 0;
	ID++;
	return ID;
}

static std::vector<FontLayout> Fonts;
gpbRMResourceName openFont(const char* fileName, gpbRMResourceName textureName)
{
	FontLayout cache;
	cache.textureName = textureName;
	cache.fontName.emplace<gpbUINT32>(getResourceName());
	cache.charactersInfo = std::move(gpbBinaryFontInfo(fileName).characters);	

	// Add font to table and return name for user.
	Fonts.push_back(cache);
	return cache.fontName;
}

static std::vector<CharacterInfo> charactersBuffer;
static gpbUINT32 currentFontLayoutID{};

void setFontType(gpbRMResourceName fontName)
{
	for (gpbUINT32 i = 0; i < Fonts.size(); i++)
	{
		if (Fonts[i].fontName == fontName)
		{
			currentFontLayoutID = i;
			break;
		}
	}
}
void setTextPos(gpbVECTOR2 posOffset)
{
	textRenderingInfo.posOffset = posOffset;
}
void setTextColor(const gpbVECTOR4& color)
{
	textRenderingInfo.color = color;
}
void setTextSize(gpbFLOAT32 size)
{
	textRenderingInfo.size = size;
}
void setTextDepth(gpbFLOAT32 depth)
{
	textRenderingInfo.depth = depth;
}
void setTextJustification(bool justification)
{
	textRenderingInfo.justify = justification;
}
void renderText(const char* text, gpbRMResourceName shadersPipelineName)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const auto strLength = strlen(text);
	if (!strLength)
		return;

	// Resize buffer if needed.
	if (strLength > charactersBuffer.size())
		charactersBuffer.resize(strLength);

	// Copy font properties to cache.
	auto fontCache = Fonts[currentFontLayoutID];

	// Compute basic characters' positions.
	gpbFLOAT32 lastCharacterWidth{};
	{
		auto posCache = textRenderingInfo.posOffset;
		for (gpbUINT32 i = 0; i < strLength; i++)
		{
			auto symbolID = gpbUINT32(unsigned char(text[i]));
			charactersBuffer[i].symbolID = symbolID;
			charactersBuffer[i].posOffset = posCache;
			lastCharacterWidth = 0.34f * fontCache.charactersInfo[symbolID] * textRenderingInfo.size;
			posCache = posCache + gpbVECTOR2(lastCharacterWidth, 0.0f);
		}
	}
	// Correct it if needed.
	{
		if (textRenderingInfo.justify)
		{
			auto difference = ((charactersBuffer[strLength - 1].posOffset.x + lastCharacterWidth) - charactersBuffer[0].posOffset.x) * 0.5f;
			for (gpbUINT32 i = 0; i < strLength; i++)
			{
				charactersBuffer[i].posOffset.x -= difference;
			}
		}		
	}

	// Update necessary buffers.
	{
		auto mappedBuf = glMapNamedBuffer(textRenderingInfoBuffer, GL_WRITE_ONLY);
		std::memcpy(mappedBuf, &textRenderingInfo, sizeof(TextRenderingInfo));
		glUnmapNamedBuffer(textRenderingInfoBuffer);
	}

	// Bind resources to pipeline.
	glUseProgram(std::get<gpbUINT32>(shadersPipelineName));
	glBindVertexArray(UIElementVAO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, fontCoordsBuffer);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, textRenderingInfoBuffer, 0, 20);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, characterInfoBuffer);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, textRenderingInfoBuffer, 16, 16);
	
	glBindTextureUnit(0, std::get<gpbUINT32>(fontCache.textureName));

	// Render.
	for (gpbUINT32 i = 0; i < strLength; i++)
	{
		// Update character buffer.
		{
			auto bufPtr = glMapNamedBuffer(characterInfoBuffer, GL_WRITE_ONLY);
			std::memcpy(bufPtr, &charactersBuffer[i], sizeof(CharacterInfo));
			glUnmapNamedBuffer(characterInfoBuffer);
		}
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

static gpbRegisterEntry currentRegistry{};
gpbRegisterEntry registerUIElement()
{
	currentRegistry++;
	return currentRegistry;
}
void renderUIRegister(gpbVECTOR3 posOffset, gpbFLOAT32 size, gpbRMResourceName shadersPieplineName, gpbRMResourceName textureName, gpbRegisterEntry regEntry)
{
	// Update UI Element Render Info
	{
		struct bufContent
		{
			gpbVECTOR3 posOffset;
			gpbFLOAT32 size;
			gpbUINT32 shouldFlip[4];
			gpbVECTOR4 overrideColor;
		};
		bufContent cache
		{
			posOffset,
			size,
			0, 0, 0, 0
		};
		auto mappedBuf = glMapNamedBuffer(UIElementRenderInfoBuffer, GL_WRITE_ONLY);
		std::memcpy(mappedBuf, &cache, sizeof(bufContent));
		glUnmapNamedBuffer(UIElementRenderInfoBuffer);
	}
	// Update Register Entry Info.
	{
		struct bufContent
		{
			gpbRegisterEntry entry;
			gpbUINT32 internal[3]{};
		};
		bufContent cache
		{
			regEntry
		};
		auto mappedBuf = glMapNamedBuffer(selectionRegisterEntryBuffer, GL_WRITE_ONLY);
		std::memcpy(mappedBuf, &cache, 16);
		glUnmapNamedBuffer(selectionRegisterEntryBuffer);
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, getNamedTextureLayout(textureName).infoBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, displayInfoBuffer);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, UIElementRenderInfoBuffer, 0, 32);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, selectionRegisterEntryBuffer, 0, 4);

	glBindVertexArray(UIElementVAO);
	glUseProgram(std::get<gpbUINT32>(shadersPieplineName));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
gpbRegisterEntry getPointedEntry(gpbVECTOR2 cursorPos)
{
	if (cursorPos.x < 0 || cursorPos.x >= displayDimensions.x || cursorPos.y < 0 || cursorPos.y >= displayDimensions.y) 
		return gpbRegisterEntry(0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, selectionFramebuffer);
	gpbRegisterEntry x;
	glReadPixels(cursorPos.x, (displayDimensions.y - cursorPos.y), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &x);
	return x;
}
static std::vector<GeometryInfo> models{};
gpbRMResourceName createModel(const gpbRMGeometryInfo& info)
{
	GeometryInfo cache{};
	gpbUINT32& VAO = std::get<gpbUINT32>(cache.VAO);
	glCreateVertexArrays(1, &VAO);
	cache.verticesCount = info.verticesCount;

	// Create buffers.
	glCreateBuffers(1, &cache.posBuffer);
	glNamedBufferStorage(cache.posBuffer, info.positions.dataSize, info.positions.data.get(), NULL);

	// Bind buffers.
	glVertexArrayVertexBuffer(VAO, 0, cache.posBuffer, 0, 12);
	
	// Bind attribs.
	glVertexArrayAttribBinding(VAO, 0, 0);

	// Format attribs.
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	
	// Enable attribs.
	glEnableVertexArrayAttrib(VAO, 0);

	models.push_back(cache);
	return cache.VAO;
}
void allocateStaticActor(gpbStaticActor& actor)
{
	
}
void setCamera(const gpbClientCamera& camera)
{
	auto mapped = glMapNamedBuffer(cameraBuffer, GL_WRITE_ONLY);
	// MULTIPLYING MATRICES IS TO REFACTOR!!
	auto cameraMatrix = camera.view;
	cameraMatrix = cameraMatrix * camera.projection;
	std::memcpy(mapped, cameraMatrix.get(), 64);
	glUnmapNamedBuffer(cameraBuffer);
}