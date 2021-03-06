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

#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "../Ngine.h"

#include "../Resource.h"
#include "Texture2D.h"

#if defined(GRAPHICS_OPENGL21) || defined(GRAPHICS_OPENGL33) || defined(GRAPHICS_OPENGLES2)
#include "OpenGL/Framebuffer.h"
#endif

namespace NerdThings::Ngine::Graphics {
    /*
     * A 2D Texture in the GPU that can be rendered to
     */
    struct NEAPI RenderTarget : public IResource {
    private:
        // Private Fields

        /*
         * The texture that the render target renders to.
         */
        Texture2D *_Texture;
    public:
        // Public Fields

        /*
         * Render target height
         */
        int Height = 0;

        /*
         * Internal framebuffer on the GPU
         */
#if defined(GRAPHICS_OPENGL33) || defined(GRAPHICS_OPENGL21) || defined(GRAPHICS_OPENGLES2)
        std::shared_ptr<OpenGL::GLFramebuffer> InternalFramebuffer = nullptr;
#endif

        /*
         * Render target width
         */
        int Width = 0;

        // Public Constructor(s)

        /*
         * Create a null render target
         */
        RenderTarget();

        /*
         * Create a render target
         */
        RenderTarget(int width_, int height_);

        // Destructor

        ~RenderTarget();

        // Public Methods

        /*
         * Get rendered texture
         */
        Texture2D *GetTexture();

        /*
         * Whether or not the render target is valid and usable.
         */
        bool IsValid() const override;

        /*
         * Unload the render target.
         * Invalidates all copies.
         */
        void Unload() override;

        // Operators

        bool operator==(const RenderTarget &b) const;

        bool operator!=(const RenderTarget &b) const;
    };
}

#endif //RENDERTARGET_H
