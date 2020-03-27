/**********************************************************************************************
 *
 *   Ngine - A 2D game engine.
 *
 *   Copyright 2020 NerdThings (Reece Mackie)
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#include "ngine/graphics/platform/opengl_graphics_device.hpp"

#if defined(NGINE_ENABLE_OPENGL) || defined(NGINE_ENABLE_OPENGLES)

#include "ngine/console.hpp"
#include "ngine/window.hpp"

#if !defined(PLATFORM_UWP)

#include <glad/glad.h>

#define GLAD
#endif

#if defined(PLATFORM_DESKTOP)

#include <GLFW/glfw3.h>

#define GLFW
#endif

#if defined(PLATFORM_UWP)
#define EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#if defined(NGINE_ENABLE_OPENGLES) && !defined(GLAD)
// Include latest GLES header
#include <GLES3/gl31.h>
// Add GLES2 extensions
#include <GLES2/gl2ext.h>
#endif

namespace ngine::graphics::platform {
    void OpenGLGraphicsDevice::clear(Color color) {
        glClearColor(color.R, color.G, color.B, color.A);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLGraphicsDevice::bindVertexArray(VertexArray *array) {
        glBindVertexArray(array->GLID);
        _prepareVertexArray(array);
        m_currentVAO = array;
    }

    void OpenGLGraphicsDevice::bindTexture(unsigned int unit, Texture2D *texture) {
        // Check unit limit
        if (unit >= 8)
            Console::fail("OpenGL", "Ngine limits the number of texture units to 8.");

        // Set the unit we are using
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture->GLID);
    }

    void OpenGLGraphicsDevice::drawPrimitives(PrimitiveType primitiveType, int start, int count) {
        GLenum prim;
        switch (primitiveType) {
            case PrimitiveType::TriangleList:
                prim = GL_TRIANGLES;
                break;
            case PrimitiveType::TriangleStrip:
                prim = GL_TRIANGLE_STRIP;
                break;
            case PrimitiveType::LineList:
                prim = GL_LINE;
                break;
            case PrimitiveType::LineStrip:
                prim = GL_LINE_STRIP;
                break;
        }
        glDrawArrays(prim, start, count);
    }

    void OpenGLGraphicsDevice::free(GraphicsResource *resource) {
        m_freeLock.lock();
        m_freeNextFrame.push_back(resource);
        m_freeLock.unlock();
    }

    OpenGLGraphicsDevice::OpenGLGraphicsDevice(Window *window) : GraphicsDevice(window) {
        // Make window context current
        m_window->makeCurrent();

        // Create context
        m_context = new OpenGLContext(m_window);

        // Determine if we're running GLES
#if !defined(GLAD)
        if (window->getContextDescriptor().Type == ContextType::OpenGLES) {
            auto major = window->getContextDescriptor().MajorVersion;
            m_isGLES2 = major == 2;
            m_isGLES3 = major == 3;
        }
#else
        m_isGLES2 = GLAD_GL_ES_VERSION_2_0 && !GLAD_GL_ES_VERSION_3_0 &&
                    !GLAD_GL_ES_VERSION_3_1 && !GLAD_GL_ES_VERSION_3_2;
        m_isGLES3 = GLAD_GL_ES_VERSION_3_0;
#endif

        // Setup viewport
        glViewport(0, 0, m_window->getWidth(), m_window->getHeight());

        // TODO: Load capabilities/extensions
    }

    OpenGLGraphicsDevice::~OpenGLGraphicsDevice() {}

    void OpenGLGraphicsDevice::_initBuffer(Buffer *buffer, int size, int count) {
        // Create
        buffer->GLID = 0;
        glGenBuffers(1, &buffer->GLID);

        // Write default size.
        _bindBuffer(buffer);

        GLenum bufType, bufUsage;
        switch (buffer->Type) {
            case BufferType::Vertex:
                bufType = GL_ARRAY_BUFFER;
                break;
            case BufferType::Index:
                bufType = GL_ELEMENT_ARRAY_BUFFER;
                break;
        }

        switch (buffer->Usage) {
            case BufferUsage::Static:
                bufUsage = GL_STATIC_DRAW;
                break;
            case BufferUsage::Dynamic:
                bufUsage = GL_DYNAMIC_DRAW;
                break;
            case BufferUsage::Stream:
                bufUsage = GL_STREAM_DRAW;
                break;
        }
        glBufferData(bufType, size * count, nullptr, bufUsage);
    }

    void OpenGLGraphicsDevice::_bindBuffer(Buffer *buffer) {
        switch (buffer->Type) {
            case BufferType::Vertex:
                glBindBuffer(GL_ARRAY_BUFFER, buffer->GLID);
                break;
            case BufferType::Index:
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->GLID);
                break;
        }
    }

    void OpenGLGraphicsDevice::_writeBuffer(Buffer *buffer, void *data, int count) {
        // Bind
        _bindBuffer(buffer);

        // Get type and usage
        GLenum bufType, bufUsage;
        switch (buffer->Type) {
            case BufferType::Vertex:
                bufType = GL_ARRAY_BUFFER;
                break;
            case BufferType::Index:
                bufType = GL_ELEMENT_ARRAY_BUFFER;
                break;
        }

        // Write (we don't use glBufferData as we want to restrict buffer size for parity with DX11).
        glBufferSubData(bufType, 0, buffer->Size * count, data);
    }

    void OpenGLGraphicsDevice::_initShader(Shader *shader, const std::string &source) {
        // Create shader
        GLenum type;
        switch (shader->Type) {
            case ShaderType::Vertex:
                type = GL_VERTEX_SHADER;
                break;
            case ShaderType::Fragment:
                type = GL_FRAGMENT_SHADER;
                break;
        }
        shader->GLID = glCreateShader(type);

        // Set source
        auto src = source.c_str();
        glShaderSource(shader->GLID, 1, &src, nullptr);

        // Compile
        glCompileShader(shader->GLID);

        // Check for compile errors
        int params = -1;
        glGetShaderiv(shader->GLID, GL_COMPILE_STATUS, &params);
        if (GL_TRUE != params) {
            int max_length = 2048;
            int actual_length = 0;
            char shader_log[2048];
            glGetShaderInfoLog(shader->GLID, max_length, &actual_length, shader_log);
            Console::notice("OpenGL", "Shader info log for shader %u:\n%s", shader->GLID, shader_log);
            Console::fail("OpenGL", "Failed to compile shader %u.", shader->GLID);
        }
        Console::notice("OpenGL", "Successfully created and compiled shader %u.", shader->GLID);
    }

    void OpenGLGraphicsDevice::_initShaderProgram(ShaderProgram *prog) {
        prog->GLID = glCreateProgram();
        Console::notice("OpenGL", "Successfully created shader program %u.", prog->GLID);
    }

    void OpenGLGraphicsDevice::_shaderProgramAttach(ShaderProgram *prog, Shader *shader) {
        glAttachShader(prog->GLID, shader->GLID);
    }

    void OpenGLGraphicsDevice::_linkShaderProgram(ShaderProgram *prog) {
        // Link
        glLinkProgram(prog->GLID);

        // Check for errors
        int params = -1;
        glGetProgramiv(prog->GLID, GL_LINK_STATUS, &params);
        if (GL_TRUE != params) {
            int max_length = 2048;
            int actual_length = 0;
            char program_log[2048];
            glGetProgramInfoLog(prog->GLID, max_length, &actual_length, program_log);
            Console::notice("OpenGL", "Program info log for %u:\n%s", prog->GLID, program_log);
            Console::fail("OpenGL", "Failed to link shader program %u.", prog->GLID);
        }
        Console::notice("OpenGL", "Successfully linked shader program %u.", prog->GLID);
    }

    void OpenGLGraphicsDevice::_useShaderProgram(ShaderProgram *prog) {
        // Use the program
        glUseProgram(prog->GLID);

        // Save current and last programs
        m_lastShaderProgram = m_currentShaderProgram;
        m_currentShaderProgram = prog;

        // Setup VAO if available.
        if (m_currentVAO)
            _prepareVertexArray(m_currentVAO);

        // Setup texture uniforms (TODO: Do we need to do this every time?)
        int *samplers = new int[prog->ExpectedSamplerCount];
        for (auto i = 0; i < prog->ExpectedSamplerCount; i++)
            samplers[i] = i;
        int loc = glGetUniformLocation(prog->GLID, prog->GLSLSamplerUniform.c_str());
        glUniform1iv(loc, prog->ExpectedSamplerCount, samplers);
        delete[] samplers;
    }

    void OpenGLGraphicsDevice::_initVertexArray(VertexArray *array) {
        // Create VAO
        array->GLID = 0;
        glGenVertexArrays(1, &array->GLID);
        _prepareVertexArray(array);
    }

    void OpenGLGraphicsDevice::_prepareVertexArray(VertexArray *array) {
        // Bind
        glBindVertexArray(array->GLID);

        // Bind buffers
        _bindBuffer(array->getVertexBuffer());
        if (array->getIndexBuffer())
            _bindBuffer(array->getIndexBuffer());

        // Add to cache if missing
        if (m_VAOShaderCache.find(array) == m_VAOShaderCache.end()) {
            m_VAOShaderCache.insert({array, nullptr});
        }

        // If there is no shader, we cannot perform element layouts yet.
        // If we're using the same shader as the last setup, skip
        if (!m_currentShaderProgram || m_VAOShaderCache[array] == m_currentShaderProgram)
            return;

        // Get elements
        auto elements = array->getLayout().Elements;
        auto size = array->getLayout().getSize();

        if (m_lastShaderProgram) {
            for (const auto &e : elements) {
                int loc = glGetAttribLocation(m_lastShaderProgram->GLID, e.Name);
                glDisableVertexAttribArray(loc);
            }
        }

        // Don't configure if mismatched
        if (array->getLayout() != m_currentShaderProgram->getLayout()) {
            // Only warn, as it may be an accident/they set the shader then the array.
            // TODO: Should we actually warn about this?
            // Console::warn("OpenGL", "Cannot configure a vertex array to use a shader with a different layout. Not configuring.");
            return;
        }

        // Prepare layout
        int offset = 0;
        for (const auto &e : elements) {
            GLenum type;
            switch (e.Type) {
//                case ElementType::Byte:
//                    type = GL_BYTE;
//                    break;
//                case ElementType::UnsignedByte:
//                    type = GL_UNSIGNED_BYTE;
//                    break;
//                case ElementType::Short:
//                    type = GL_SHORT;
//                    break;
//                case ElementType::UnsignedShort:
//                    type = GL_UNSIGNED_SHORT;
//                    break;
                case ElementType::Int:
                    type = GL_INT;
                    break;
                case ElementType::UnsignedInt:
                    type = GL_UNSIGNED_INT;
                    break;
                case ElementType::Float:
                    type = GL_FLOAT;
                    break;
            }

            GLint count = e.Count;
            if (count > 4)
                Console::fail("OpenGL", "VertexBufferElement count cannot be greater than 4.");

            // Get location in shader
            int loc = glGetAttribLocation(m_currentShaderProgram->GLID, e.Name);

            // Enable attrib
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, count, type, e.Normalized ? GL_TRUE : GL_FALSE, size,
                                  (void *) offset);
            offset += e.getSize();
        }

        // Register in cache
        m_VAOShaderCache[array] = m_currentShaderProgram;
    }

    void OpenGLGraphicsDevice::_initTexture(Texture2D *texture, void *data) {
        // Create texture
        texture->GLID = 0;
        glGenTextures(1, &texture->GLID);

        // Bind texture and set it up (always use unit 0 as it should be replaced when drawing commences).
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GLID);

        // Write image data
        GLenum internalFormat = 0, format = 0, type = 0;
        switch (texture->getPixelFormat()) {
            case PixelFormat::R8G8B8A8:
                internalFormat = GL_RGBA;
                format = GL_RGBA;
                type = GL_UNSIGNED_BYTE;
                break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture->getWidth(), texture->getHeight(), 0, format, type, data);

        // TODO: Texture parameters
        // TODO: Mipmap limits
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        Console::notice("OpenGL", "Successfully created texture %u.", texture->GLID);
    }

    void OpenGLGraphicsDevice::_freeResource(GraphicsResource *resource) {
        // Free resource.
        switch (resource->getResourceType()) {
            case ResourceType::Buffer:
                glDeleteBuffers(1, &resource->GLID);
                break;
            case ResourceType::Shader:
                glDeleteShader(resource->GLID);
                break;
            case ResourceType::ShaderProgram:
                // TODO: Handle the VAO system if the current shader is removed.
                glDeleteProgram(resource->GLID);
                break;
            case ResourceType::VertexArray:
                glDeleteVertexArrays(1, &resource->GLID);
                // Remove from VAO cache
                m_VAOShaderCache.erase((VertexArray *) resource);
                break;
        }
        resource->GLID = 0;
    }

    void OpenGLGraphicsDevice::_present() {
        // SwapBuffers
        m_context->swapBuffers();

        // Dispose resources
        for (auto res : m_freeThisFrame)
            _freeResource(res);
        m_freeThisFrame.clear();

        m_freeLock.lock();
        {
            auto tmp = m_freeThisFrame;
            m_freeThisFrame = m_freeNextFrame;
            m_freeNextFrame = m_freeThisFrame;
        }
        m_freeLock.unlock();
    }

    void OpenGLGraphicsDevice::_onResize() {
        glViewport(0, 0, m_window->getWidth(), m_window->getHeight());
    }
}

#endif // defined(NGINE_ENABLE_OPENGL) || defined(NGINE_ENABLE_OPENGLES)
