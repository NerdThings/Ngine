/**********************************************************************************************
*
*   Ngine - A (mainly) 2D game engine.
*   
*   CONFIGURATION:
*   
*   - #define INCLUDE_RAYLIB
*       Include the raylib header
*   
*   Copyright (C) 2019 NerdThings
*
*   LICENSE: Apache License 2.0
*   View: https://github.com/NerdThings/Ngine/blob/master/LICENSE
*
**********************************************************************************************/

#ifndef NGINE_H
#define NGINE_H

//----------------------------------------------------------------------------------
// Basic Definitions
//----------------------------------------------------------------------------------

#if defined(NGINE_EXPORTS)
#define INCLUDE_RAYLIB
#endif

#if defined(_WIN32) && defined(NGINE_EXPORTS)
#define NEAPI __declspec(dllexport)         // Windows DLL Export
#elif defined(_WIN32)
#define NEAPI __declspec(dllimport)         // Windows DLL Import
#else
#define NEAPI
#endif

// Use raylib DLL
#define USE_LIBTYPE_SHARED

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------

// C++ Includes
#include <algorithm>                // Required for: std::clamp and alike methods
#include <chrono>                   // Required for: std::chrono timings
#include <string>                   // Required for: std::string
#include <map>                      // Required for: std::map
#include <thread>                   // Required for: std::this_thread sleep
#include <vector>                   // Required for: std::vector

// Library Includes
#if defined(INCLUDE_RAYLIB)
#include "3rd-party/raylib/src/raylib.h"
#endif

// Type includes
#if !defined(TYPE_DECL_HEADER)
#include "Types/Camera2D.h"
#include "Types/Color.h"
#include "Types/Font.h"
#include "Types/Rectangle.h"
#include "Types/Texture2D.h"
#include "Types/Vector2.h"
#endif

//----------------------------------------------------------------------------------
// Using Namespaces
//----------------------------------------------------------------------------------

#if defined(NGINE_EXPORTS) && !defined(TYPE_DECL_HEADER)
using namespace Ngine::Types;
#endif

#endif // NGINE_H
