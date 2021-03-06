/**********************************************************************************************
*
*   Ngine - The 2D game engine.
*
*   Copyright (C) 2019 NerdThings
*
*   LICENSE: Apache License 2.0
*   View: https://github.com/NerdThings/Ngine/blob/master/LICENSE
*
**********************************************************************************************/

#if defined(GRAPHICS_OPENGLES2) || defined(GRAPHICS_OPENGL21) || defined(GRAPHICS_OPENGL33)
#include "Buffer.h"

// Platform specifics
#if defined(GRAPHICS_OPENGL33) || defined(GRAPHICS_OPENGL21)
#include <glad/glad.h>
#elif defined(GRAPHICS_OPENGLES2)
#define GL_KHR_debug 0
#define GL_GLEXT_PROTOTYPES 1 // UWP???
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#endif

#if defined(PLATFORM_DESKTOP)
#include <GLFW/glfw3.h>
#elif defined(PLATFORM_UWP)
#include <angle_windowsstore.h>
#endif

namespace NerdThings::Ngine::Graphics::OpenGL {
    GLBuffer::GLBuffer(GLBufferType type_, GLBufferUsage usage_) : _Type(type_), _Usage(usage_) {
        // Generate buffer
        glGenBuffers(1, &ID);

        // Logging
        ConsoleMessage("Created new buffer with ID " + std::to_string(ID) + ".", "NOTICE", "GLBuffer");
    }

    GLBuffer::~GLBuffer() {
        // Delete buffer
        glDeleteBuffers(1, &ID);
        ConsoleMessage("Deleted buffer with ID " + std::to_string(ID) + ".", "NOTICE", "GLBuffer");

        // Set ID to 0
        ID = 0;
    }

    void GLBuffer::Bind() {
        // Bind buffer
        glBindBuffer(_Type, ID);
    }

    GLBufferType GLBuffer::GetBufferType() {
        return _Type;
    }

    GLBufferUsage GLBuffer::GetBufferUsage() {
        return _Usage;
    }

    void GLBuffer::SetData(void *data_, int dataSize_) {
        // Bind
        Bind();

        // Set data
        glBufferData(_Type, dataSize_, data_, _Usage);
    }
}
#endif
