/**********************************************************************************************
*
*   Ngine - A 2D game engine.
*
*   Copyright (C) 2020 NerdThings.
*
*   LICENSE: GNU LGPLv3
*   View: In Ngine.hpp
*
**********************************************************************************************/

#ifndef NEW_SHADER_H
#define NEW_SHADER_H

#include "../Config.hpp"

#ifdef USE_EXPERIMENTAL_RENDERER

#include "../Resource.hpp"

namespace NerdThings::Ngine::Graphics {
    class ShaderProgram;

    class NEAPI Shader : public IResource {
        // Set ShaderProgram as a friend.
        friend class ShaderProgram;

        /**
         * Vertex shader ID.
         */
        unsigned int _VertexShader;

        /**
         * Fragment shader ID.
         */
        unsigned int _FragmentShader;

        /**
         * Whether or not the shader is compiled
         */
        bool _Compiled = false;

        /**
         * Compile the given shader.
         */
        bool __Compile(unsigned int id_);

    public:
        /**
         * Create a shader with source strings.
         *
         * @param vertSrc_
         * @param fragSrc_
         */
        Shader(std::string vertSrc_, std::string fragSrc_);
        ~Shader();

        /**
         * Compile the shader.
         *
         * @return Whether or not the shader compiled successfully.
         */
        bool Compile();

        /**
         * Determine if the shader is valid.
         *
         * @note If the shader has not compiled, or failed to compile it will not be valid.
         * @return Whether or not the shader is valid.
         */
        bool IsValid() const override;

        /**
         * Delete the shader.
         */
        void Unload() override;
    };
}

#endif

#endif //NEW_SHADER_H
