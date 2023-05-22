#pragma once
#include "../../include/gpbGlobalDefs.hpp"

#ifndef MODULE_DEBUG_INFO
// Comment this define when you are compiling release version.
#define MODULE_DEBUG_INFO
#include <iostream>
#endif

#ifdef _WIN64
#include <Windows.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#endif

void openContext(gpbWindowPtr window);
void swap();
void queryExtensions();
void closeContext();

/* EXTENSIONS FUNCTIONS */
extern PFNGLGETSTRINGIPROC glGetStringi;

/* TEXTURE PROCESSING FUNCTION PTRS */
extern PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
extern PFNGLCREATETEXTURESPROC glCreateTextures;
extern PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D;
extern PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glCompressedTextureSubImage2D;
extern PFNGLTEXTUREPARAMETERIPROC glTextureParameteri;
extern PFNGLDELETETEXTURESPROC glDeleteTextures;

/* SHADERS PROCESSING FUNCTION PTRS */
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;

/* VERTEX ARRAYS PROCESSING FUNCTION PTRS */
extern PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
extern PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
extern PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat;
extern PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib;

/* BUFFERS PROCESSING FUNCTION PTRS */
extern PFNGLCREATEBUFFERSPROC glCreateBuffers;
extern PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
extern PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData;
extern PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer;
extern PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer;
extern PFNGLBINDBUFFERBASEPROC glBindBufferBase;
extern PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;

/* FRAMEBUFFERS PROCESSING FUNCTION PTRS */
extern PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer;
extern PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer;
extern PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture;
extern PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer;
extern PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus;
extern PFNGLREADPIXELSPROC glReadPixels;

/* RENDERBUFFERS PROCESSING FUNCTION PTRS */
extern PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
extern PFNGLNAMEDRENDERBUFFERSTORAGEPROC glNamedRenderbufferStorage;

/* BASIC FUNCTIONS */
extern PFNGLVIEWPORTPROC glViewport;
extern PFNGLCLEARCOLORPROC glClearColor;
extern PFNGLCLEARPROC glClear;
extern PFNGLGETINTEGERVPROC glGetIntegerv;
extern PFNGLENABLEPROC glEnable;
extern PFNGLDISABLEPROC glDisable;
extern PFNGLDRAWARRAYSPROC glDrawArrays;
extern PFNGLDEPTHFUNCPROC glDepthFunc;
extern PFNGLBLENDFUNCPROC glBlendFunc;