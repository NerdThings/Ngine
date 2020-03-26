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

#ifndef NGINE_CONTEXT_DESCRIPTOR_HPP
#define NGINE_CONTEXT_DESCRIPTOR_HPP

#include "ngine/config.hpp"

namespace ngine::graphics {
    /**
     * Context type.
     */
    enum class ContextType {
        /**
         * OpenGL context.
         */
        OpenGL,
        OpenGLES,
        DirectX,
        Vulkan
    };

    /**
     * Context version information.
     */
    struct ContextDescriptor {
        /**
         * Default context version for the current platform.
         */
        static ContextDescriptor Default;

        /**
         * Context type.
         */
        ContextType Type;

        /**
         * Major version (only for OGL at the minute).
         */
        int MajorVersion;

        /**
         * Minor version (only for OGL at the minute).
         */
        int MinorVersion;

        /**
         * Verify context descriptor options.
         */
        bool verify() const;
    };
}

#endif //NGINE_CONTEXT_DESCRIPTOR_HPP