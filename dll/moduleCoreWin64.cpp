#ifdef _WIN64
#include "../../include/gpbExceptions.hpp"
#include "../include/API.hpp"
#include <GL/wglext.h>
#include <vector>
#include <string>

/* EXTENSIONS FUNCTIONS */
 PFNGLGETSTRINGIPROC glGetStringi{};

/* TEXTURE PROCESSING FUNCTION PTRS */
 PFNGLBINDTEXTUREUNITPROC glBindTextureUnit{};
 PFNGLCREATETEXTURESPROC glCreateTextures{};
 PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D{};
 PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glCompressedTextureSubImage2D{};
 PFNGLTEXTUREPARAMETERIPROC glTextureParameteri{};
 PFNGLDELETETEXTURESPROC glDeleteTextures;

/* SHADERS PROCESSING FUNCTION PTRS */
 PFNGLUSEPROGRAMPROC glUseProgram{};
 PFNGLCREATESHADERPROC glCreateShader{};
 PFNGLATTACHSHADERPROC glAttachShader{};
 PFNGLCOMPILESHADERPROC glCompileShader{};
 PFNGLSHADERSOURCEPROC glShaderSource{};
 PFNGLLINKPROGRAMPROC glLinkProgram{};
 PFNGLDELETESHADERPROC glDeleteShader{};
 PFNGLDELETEPROGRAMPROC glDeleteProgram{};
 PFNGLCREATEPROGRAMPROC glCreateProgram{};

/* VERTEX ARRAYS PROCESSING FUNCTION PTRS */
 PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays{};
 PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays{};
 PFNGLBINDVERTEXARRAYPROC glBindVertexArray{};
 PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer{};
 PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding{};
 PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat{};
 PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib{};

/* BUFFERS PROCESSING FUNCTION PTRS */
 PFNGLCREATEBUFFERSPROC glCreateBuffers{};
 PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage{};
 PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData{};
 PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer{};
 PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer{};
 PFNGLBINDBUFFERBASEPROC glBindBufferBase{};
 PFNGLBINDBUFFERRANGEPROC glBindBufferRange{};
 PFNGLDELETEBUFFERSPROC glDeleteBuffers{};

/* FRAMEBUFFERS PROCESSING FUNCTION PTRS */
 PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers{};
 PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers{};
 PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer{};
 PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer{};
 PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer{};
 PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture{};
 PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer{};
 PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus{};
 PFNGLREADPIXELSPROC glReadPixels{};

/* RENDERBUFFERS PROCESSING FUNCTION PTRS */
 PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers{};
 PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers{};
 PFNGLNAMEDRENDERBUFFERSTORAGEPROC glNamedRenderbufferStorage{};

 /* BASIC FUNCTIONS */
 PFNGLVIEWPORTPROC glViewport{};
 PFNGLCLEARCOLORPROC glClearColor{};
 PFNGLCLEARPROC glClear{};
 PFNGLGETINTEGERVPROC glGetIntegerv{};
 PFNGLENABLEPROC glEnable{};
 PFNGLDISABLEPROC glDisable{};
 PFNGLDRAWARRAYSPROC glDrawArrays{};
 PFNGLDEPTHFUNCPROC glDepthFunc{};
 PFNGLBLENDFUNCPROC glBlendFunc{};

 static HDC dvc{};
 static HGLRC dvcCon{};

 enum class OpenGLCoreVersion
 {
	 Old,
	 Modern
 };

 // Not all functions can be imported by wglGetProcAddress, GetProcAddress must be used. Is it driver bug...?
 template<OpenGLCoreVersion version, typename T>
 static void loadFunction(T& func, LPCSTR funcName)
 {
	 if constexpr(version == OpenGLCoreVersion::Old)
		 func = reinterpret_cast<T>(GetProcAddress(GetModuleHandleA("opengl32.dll"), funcName));
	 if constexpr(version == OpenGLCoreVersion::Modern)
		 func = reinterpret_cast<T>(wglGetProcAddress(funcName));
#ifdef MODULE_DEBUG_INFO
	 if (func == nullptr)
		 std::cout << "FAILED TO LOAD OPENGL FUNCTION: " << funcName << std::endl;
#endif
 }

 void openContext(gpbWindowPtr window)
 {
	 // Query hardware accerelated pixel.
	 {
		 PIXELFORMATDESCRIPTOR PFD{};
		 PFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		 PFD.nVersion = 1;
		 PFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		 PFD.iPixelType = PFD_TYPE_RGBA;
		 PFD.cColorBits = 32;
		 PFD.cAlphaBits = 8;
		 PFD.cDepthBits = 32;

		 dvc = GetDC(reinterpret_cast<HWND>(window));
		 SetPixelFormat(dvc, ChoosePixelFormat(dvc, &PFD), &PFD);
	 }
	 // Create temporary basic context.
	 auto temporaryDvcCon = wglCreateContext(dvc);
	 wglMakeCurrent(dvc, temporaryDvcCon);

	 // Create destinated context.
	 {
		 int  contextAttribs[] =
		 {
			 WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			 WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			 WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			 //WGL_CONTEXT_OPENGL_NO_ERROR_ARB, GL_FALSE,
			 0
		 };

		 auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
		 dvcCon = wglCreateContextAttribsARB(dvc, 0, contextAttribs);
	 }
	 // Check destinated context was created.
	 if (!dvcCon)
		 throw gpbOpenGLUnsupported();

	 // Delete unnecessary context.
	 wglMakeCurrent(dvc, dvcCon);
	 wglDeleteContext(temporaryDvcCon);

	 // Set destinated context as current.
	 wglMakeCurrent(dvc, dvcCon);

	 // Load functions.
	 loadFunction<OpenGLCoreVersion::Modern>(glBindTextureUnit, "glBindTextureUnit");
	 loadFunction<OpenGLCoreVersion::Modern>(glCreateTextures, "glCreateTextures");
	 loadFunction<OpenGLCoreVersion::Modern>(glTextureStorage2D, "glTextureStorage2D");
	 loadFunction<OpenGLCoreVersion::Modern>(glCompressedTextureSubImage2D, "glCompressedTextureSubImage2D");
	 loadFunction<OpenGLCoreVersion::Modern>(glTextureParameteri, "glTextureParameteri");
	 loadFunction<OpenGLCoreVersion::Old>(glDeleteTextures, "glDeleteTextures");

	 loadFunction<OpenGLCoreVersion::Modern>(glUseProgram, "glUseProgram");
	 loadFunction<OpenGLCoreVersion::Modern>(glCreateShader, "glCreateShader");
	 loadFunction<OpenGLCoreVersion::Modern>(glAttachShader, "glAttachShader");
	 loadFunction<OpenGLCoreVersion::Modern>(glCompileShader, "glCompileShader");
	 loadFunction<OpenGLCoreVersion::Modern>(glShaderSource, "glShaderSource");
	 loadFunction<OpenGLCoreVersion::Modern>(glLinkProgram, "glLinkProgram");
	 loadFunction<OpenGLCoreVersion::Modern>(glDeleteShader, "glDeleteShader");
	 loadFunction<OpenGLCoreVersion::Modern>(glDeleteProgram, "glDeleteProgram");
	 loadFunction<OpenGLCoreVersion::Modern>(glCreateProgram, "glCreateProgram");

	 loadFunction<OpenGLCoreVersion::Modern>(glCreateVertexArrays, "glCreateVertexArrays");
	 loadFunction<OpenGLCoreVersion::Modern>(glDeleteVertexArrays, "glDeleteVertexArrays");
	 loadFunction<OpenGLCoreVersion::Modern>(glBindVertexArray, "glBindVertexArray");
	 loadFunction<OpenGLCoreVersion::Modern>(glVertexArrayVertexBuffer, "glVertexArrayVertexBuffer");
	 loadFunction<OpenGLCoreVersion::Modern>(glVertexArrayAttribBinding, "glVertexArrayAttribBinding");
	 loadFunction<OpenGLCoreVersion::Modern>(glVertexArrayAttribFormat, "glVertexArrayAttribFormat");
	 loadFunction<OpenGLCoreVersion::Modern>(glEnableVertexArrayAttrib, "glEnableVertexArrayAttrib");

	 loadFunction<OpenGLCoreVersion::Modern>(glCreateBuffers, "glCreateBuffers");
	 loadFunction<OpenGLCoreVersion::Modern>(glNamedBufferStorage, "glNamedBufferStorage");
	 loadFunction<OpenGLCoreVersion::Modern>(glNamedBufferSubData, "glNamedBufferSubData");
	 loadFunction<OpenGLCoreVersion::Modern>(glMapNamedBuffer, "glMapNamedBuffer");
	 loadFunction<OpenGLCoreVersion::Modern>(glUnmapNamedBuffer, "glUnmapNamedBuffer");
	 loadFunction<OpenGLCoreVersion::Modern>(glBindBufferBase, "glBindBufferBase");
	 loadFunction<OpenGLCoreVersion::Modern>(glBindBufferRange, "glBindBufferRange");
	 loadFunction<OpenGLCoreVersion::Modern>(glDeleteBuffers, "glDeleteBuffers");

	 loadFunction<OpenGLCoreVersion::Modern>(glCreateFramebuffers, "glCreateFramebuffers");
	 loadFunction<OpenGLCoreVersion::Modern>(glDeleteFramebuffers, "glDeleteFramebuffers");
	 loadFunction<OpenGLCoreVersion::Modern>(glBindFramebuffer, "glBindFramebuffer");
	 loadFunction<OpenGLCoreVersion::Modern>(glNamedFramebufferDrawBuffer, "glNamedFramebufferDrawBuffer");
	 loadFunction<OpenGLCoreVersion::Modern>(glNamedFramebufferReadBuffer, "glNamedFramebufferReadBuffer");
	 loadFunction<OpenGLCoreVersion::Modern>(glNamedFramebufferTexture, "glNamedFramebufferTexture");
	 loadFunction<OpenGLCoreVersion::Modern>(glNamedFramebufferRenderbuffer, "glNamedFramebufferRenderbuffer");
	 loadFunction<OpenGLCoreVersion::Modern>(glCheckNamedFramebufferStatus, "glCheckNamedFramebufferStatus");
	 loadFunction<OpenGLCoreVersion::Old>(glReadPixels, "glReadPixels");

	 loadFunction<OpenGLCoreVersion::Modern>(glCreateRenderbuffers, "glCreateRenderbuffers");
	 loadFunction<OpenGLCoreVersion::Modern>(glDeleteRenderbuffers, "glDeleteRenderbuffers");
	 loadFunction<OpenGLCoreVersion::Modern>(glNamedRenderbufferStorage, "glNamedRenderbufferStorage");

	 loadFunction<OpenGLCoreVersion::Old>(glViewport, "glViewport");
	 loadFunction<OpenGLCoreVersion::Old>(glClearColor, "glClearColor");
	 loadFunction<OpenGLCoreVersion::Old>(glClear, "glClear");
	 loadFunction<OpenGLCoreVersion::Old>(glGetIntegerv, "glGetIntegerv");
	 loadFunction<OpenGLCoreVersion::Old>(glEnable, "glEnable");
	 loadFunction<OpenGLCoreVersion::Old>(glDisable, "glDisable");
	 loadFunction<OpenGLCoreVersion::Old>(glDrawArrays, "glDrawArrays");
	 loadFunction<OpenGLCoreVersion::Old>(glDepthFunc, "glDepthFunc");
	 loadFunction<OpenGLCoreVersion::Old>(glBlendFunc, "glBlendFunc");
 }
 void queryExtensions()
 {
	 // Firstly load necessary function.
	 loadFunction<OpenGLCoreVersion::Modern>(glGetStringi, "glGetStringi");

	 // Get count of available extensions.
	 gpbINT32 extensionsCount{};
	 glGetIntegerv(GL_NUM_EXTENSIONS, &extensionsCount);

	 // List extensions to query.
	 std::vector<std::string> extensionsToQuery
	 {
		 "GL_ARB_enhanced_layouts"
	 };
	 std::vector<bool> extensionsPresence(extensionsToQuery.size());

	 // Check support for every queried extension.
	 for (gpbUINT32 qExtID = 0; qExtID < extensionsToQuery.size(); qExtID++)
	 {
		 extensionsPresence[qExtID] = false;
		 for (gpbUINT32 extID = 0; extID < extensionsCount; extID++)
		 {
			 std::string extStr = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, extID));
			 if (extensionsToQuery[qExtID] == extStr)
			 {
				 extensionsPresence[qExtID] = true;
				 break;
			 }
		 }
	 }

	 // Check all extensions are supported.
	 for (gpbUINT32 i = 0; i < extensionsPresence.size(); i++)
	 {
		 if (!extensionsPresence[i])
			 throw gpbOpenGLUnsupported();
	 }
 }
 void closeContext()
 {
	 wglDeleteContext(dvcCon);
 }
 void swap()
 {
	 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 SwapBuffers(dvc);
 }
#endif