#if defined(GRAPHICS_OPENGLES2) || defined(GRAPHICS_OPENGL33)
#include "Framebuffer.h"

// Platform specifics
#if defined(GRAPHICS_OPENGL33)
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
    GLFramebuffer::GLFramebuffer(int width_, int height_, bool useDepth_, GLPixelFormat format_) {
        // Check inputs
        if (format_ >= COMPRESSED_DXT1_RGB)
            throw std::runtime_error("Cannot use a compressed pixel format.");

        // Check if we use the depth texture
        if (useDepth_) _HasDepthBuffer = true;

        // Create FBO
        glGenFramebuffers(1, &ID);
        Bind();

        // Color attachment
        RenderTexture = std::make_shared<GLTexture>(width_, height_, nullptr);
        RenderTexture->SetParameter(TEXPARAM_MAG_FILTER, FILTER_FUNC_NEAREST);
        RenderTexture->SetParameter(TEXPARAM_MIN_FILTER, FILTER_FUNC_NEAREST);

        // Depth buffer
        glGenRenderbuffers(1, &_DepthBufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, _DepthBufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _DepthBufferID);

        // Set color attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexture->ID, 0);

        // Unbind framebuffer
        Unbind();

        // Check this worked
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            // Message about failure
            ConsoleMessage("Failed to create framebuffer.", "WARN", "GLFramebuffer");

            // Delete texture
            RenderTexture = nullptr;

            // Delete framebuffer
            glDeleteFramebuffers(1, &ID);

            // Set ID to 0
            ID = 0;
        }
    }

    GLFramebuffer::~GLFramebuffer() {
        // Delete texture
        RenderTexture = nullptr;

        // Delete framebuffer
        glDeleteFramebuffers(1, &ID);

        // Set ID to 0
        ID = 0;
    }

    void GLFramebuffer::Bind() {
        // Bind
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
    }

    void GLFramebuffer::Unbind() {
        // Unbind
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
#endif