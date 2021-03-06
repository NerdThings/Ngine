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

#include "Window.h"

// Platform specifics
#if defined(GRAPHICS_OPENGL33)
#include <glad/glad.h>
#elif defined(GRAPHICS_OPENGLES2)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#if defined(PLATFORM_DESKTOP)
#include <GLFW/glfw3.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#elif defined(PLATFORM_UWP)
#include <angle_windowsstore.h>
#include "UWP/GameApp.h"
#endif

#include <stdexcept>

#include "Audio/AudioDevice.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Input/Gamepad.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"

namespace NerdThings::Ngine {
    // Private Fields

    int Window::_CurrentHeight = 0;
    int Window::_CurrentWidth = 0;
    bool Window::_Initialized = false;
    bool Window::_IsFullscreen = false;
    int Window::_PreFullscreenPosX = 0;
    int Window::_PreFullscreenPosY = 0;
    int Window::_PreFullscreenSizeWidth = 0;
    int Window::_PreFullscreenSizeHeight = 0;

#if defined(PLATFORM_DESKTOP) && defined(_WIN32)
    bool Window::_ConsoleAllocated = false;
    std::fstream Window::_ConsoleIn;
    std::fstream Window::_ConsoleOut;
    std::fstream Window::_ConsoleErr;
    std::streambuf *Window::_OldCinBuffer = nullptr;
    std::streambuf *Window::_OldCoutBuffer = nullptr;
    std::streambuf *Window::_OldCerrBuffer = nullptr;
#endif

    // Public Fields

    WindowConfig Window::Config = {};

#if defined(PLATFORM_DESKTOP)
    void *Window::WindowPtr = nullptr;
#elif defined(PLATFORM_UWP)
    EGLContext Window::Context = nullptr;
    EGLDisplay Window::Display = nullptr;
    EGLSurface Window::Surface = nullptr;
#endif

    // Public Methods

    void Window::ApplyConfig() {
        if (!_Initialized) return;

        ConsoleMessage("Updating window config.", "NOTICE", "Window");

#if defined(PLATFORM_DESKTOP)
        // V-Sync
        glfwSwapInterval(Config.VSync ? 1 : 0);

        // Window Resize
        glfwSetWindowAttrib((GLFWwindow *)WindowPtr, GLFW_RESIZABLE, Config.Resizable ? 1 : 0);

        // Window title
        glfwSetWindowTitle((GLFWwindow *)WindowPtr, Config.Title.c_str());

        // Get display info
        auto display = glfwGetPrimaryMonitor();
        auto vidmode = glfwGetVideoMode(display);

        // Toggle fullscreen
        if (Config.Fullscreen && !_IsFullscreen) {
            // Save info
            glfwGetWindowSize((GLFWwindow *)WindowPtr, &_PreFullscreenSizeWidth, &_PreFullscreenSizeHeight);
            glfwGetWindowPos((GLFWwindow *)WindowPtr, &_PreFullscreenPosX, &_PreFullscreenPosY);

            // Fullscreen
            glfwSetWindowMonitor((GLFWwindow *)WindowPtr, display, 0, 0, vidmode->width, vidmode->height, GLFW_DONT_CARE);
            _IsFullscreen = true;
        } else {
            // No fullscreen
            if (_IsFullscreen) {
                glfwSetWindowMonitor((GLFWwindow *)WindowPtr, nullptr, _PreFullscreenPosX, _PreFullscreenPosY, _PreFullscreenSizeWidth, _PreFullscreenSizeHeight, GLFW_DONT_CARE);

                _IsFullscreen = false;
            }
        }

        // Window icon
        if (Config.Icon != nullptr && Config.Icon->Format == Graphics::UNCOMPRESSED_R8G8B8A8) {
            GLFWimage icon[1] = {0};

            icon[0].width = Config.Icon->Width;
            icon[0].height = Config.Icon->Height;
            icon[0].pixels = (unsigned char *)Config.Icon->PixelData;

            glfwSetWindowIcon((GLFWwindow *)WindowPtr, 1, icon);
        }

        // Windows Console
#if defined(_WIN32)
        if (Config.NativeDebugConsole && !_ConsoleAllocated) {
            // Add console
            AllocConsole();
            SetConsoleTitle((Config.Title + ": Debug Console").c_str());

            // Save old buffers
            _OldCinBuffer = std::cin.rdbuf();
            _OldCoutBuffer = std::cout.rdbuf();
            _OldCerrBuffer = std::cerr.rdbuf();

            // Open new buffers
            _ConsoleIn.open("CONIN$", std::ios::in);
            _ConsoleOut.open("CONOUT$", std::ios::out);
            _ConsoleErr.open("CONOUT$", std::ios::out);
            std::cin.rdbuf(_ConsoleIn.rdbuf());
            std::cout.rdbuf(_ConsoleOut.rdbuf());
            std::cerr.rdbuf(_ConsoleErr.rdbuf());

            // Disable close button
            HWND hwnd = GetConsoleWindow();
            if (hwnd != nullptr)
            {
                HMENU hMenu = ::GetSystemMenu(hwnd, FALSE);
                if (hMenu != nullptr) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
            }

            // Disable Ctrl+C
            SetConsoleCtrlHandler(nullptr, true);

            // Mark as open
            _ConsoleAllocated = true;
            ConsoleMessage("Allocated a console to the game window. Output logs forwarded.", "NOTICE", "Window");
        } else if (!Config.NativeDebugConsole && _ConsoleAllocated) {
            // Close our buffers
            _ConsoleIn.close();
            _ConsoleOut.close();
            _ConsoleErr.close();

            // Reset old
            std::cin.rdbuf(_OldCinBuffer);
            std::cout.rdbuf(_OldCoutBuffer);
            std::cerr.rdbuf(_OldCerrBuffer);

            _OldCinBuffer = nullptr;
            _OldCoutBuffer = nullptr;
            _OldCerrBuffer = nullptr;

            // Release console
            FreeConsole();

            // Mark as deallocated
            _ConsoleAllocated = false;
            ConsoleMessage("Deallocated the console from the game window. Output logs restored.", "NOTICE", "Window");
        }
#endif
#elif defined(PLATFORM_UWP)
        // V-sync
        eglSwapInterval(Display, Config.VSync ? 1 : 0);

        // Fullscreen
        if (Config.Fullscreen && !_IsFullscreen) {
            Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->TryEnterFullScreenMode();
            _IsFullscreen = true;
        } else if (!Config.Fullscreen && _IsFullscreen) {
            Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->ExitFullScreenMode();
            _IsFullscreen = false;
        }
#endif
    }

    void Window::Close() {
#if defined(GRAPHICS_OPENGL21) || defined(GRAPHICS_OPENGL33) || defined(GRAPHICS_OPENGLES2)
        // Window closing, clean OpenGL state
        Graphics::OpenGL::GL::Cleanup();
        ConsoleMessage("The OpenGL state has been cleaned.", "NOTICE", "Window");
#endif

#if defined(PLATFORM_DESKTOP)
        // Destroy window
        glfwDestroyWindow((GLFWwindow*)WindowPtr);

        // Disable GLFW
        glfwTerminate();
#elif defined(PLATFORM_UWP)
        // Close surface, context and Display
        if (Display != EGL_NO_DISPLAY) {
            eglMakeCurrent(Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if (Surface != EGL_NO_SURFACE) {
                eglDestroySurface(Display, Surface);
                Surface = EGL_NO_SURFACE;
            }

            if (Context != EGL_NO_CONTEXT) {
                eglDestroyContext(Display, Context);
                Context = EGL_NO_CONTEXT;
            }

            eglTerminate(Display);
            Display = EGL_NO_DISPLAY;
        }
#endif
        ConsoleMessage("Closed window.", "NOTICE", "Window");

        // Unset width and height as the window is closed
        _CurrentWidth = 0;
        _CurrentHeight = 0;
        _Initialized = false;
    }

    int Window::GetHeight() {
        return _CurrentHeight;
    }

    int Window::GetWidth() {
        return _CurrentWidth;
    }

    void Window::Init() {
        // Init
#if defined(PLATFORM_DESKTOP)
        // Init GLFW
        if (!glfwInit()) {
            ConsoleMessage("Failed to init GLFW.", "ERROR", "Window");
            throw std::runtime_error("[Window::Init] Failed to init GLFW.");
        }
        ConsoleMessage("Initialised GLFW.", "NOTICE", "Window");

        // Set defaults
        glfwDefaultWindowHints();

        // Apply config
        if (Config.MSAA_4X) glfwWindowHint(GLFW_SAMPLES, 4);

        // Set version string
        switch(Graphics::OpenGL::GL::GetGLVersion()) {
            case Graphics::OpenGL::OPENGL_21:
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
                break;
            case Graphics::OpenGL::OPENGL_33:
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                break;
            case Graphics::OpenGL::OPENGL_ES2:
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

#if defined(PLATFORM_DESKTOP)
                glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#else
                glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
#endif
                break;
            case Graphics::OpenGL::OPENGL_UNKNOWN:
                throw std::runtime_error("Error, unable to determine an OpenGL version to use.");
        }
#endif

        // Creation
#if defined(PLATFORM_DESKTOP)
        // Create window
        WindowPtr = glfwCreateWindow(Config.InitialWidth, Config.InitialHeight, Config.Title.c_str(), nullptr, nullptr);
        if (!WindowPtr) {
            glfwTerminate();
            throw std::runtime_error("Failed to create game window.");
        }

        // Get initial size
        glfwGetWindowSize((GLFWwindow *)WindowPtr, &_CurrentWidth, &_CurrentHeight);

        // Use new context
        glfwMakeContextCurrent((GLFWwindow*) WindowPtr);
#elif defined(GRAPHICS_OPENGLES2)
        EGLint samples = 0;
        EGLint sampleBuffer = 0;

        if (Config.MSAA_4X) {
#if !defined(PLATFORM_UWP) // Seems to crash UWP, so don't
            samples = 4;
            sampleBuffer = 1;
#endif
        }

        const EGLint framebufferAttribs[] = {
            // Type of context support -> Required on RPI?
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,

            // RED color bit depth (alternative: 5)
            EGL_RED_SIZE, 8,

            // GREEN color bit depth (alternative: 6)
            EGL_GREEN_SIZE, 8,

            // BLUE color bit depth (alternative: 5)
            EGL_BLUE_SIZE, 8,

            // ALPHA bit depth (required for transparent framebuffer)
            EGL_ALPHA_SIZE, 8,

            // Depth buffer size (Required to use Depth testing!)
            EGL_DEPTH_SIZE, 16,

            // Stencil buffer size
            EGL_STENCIL_SIZE, 8,

            // Activate MSAA
            EGL_SAMPLE_BUFFERS, sampleBuffer,

            // 4x Antialiasing if activated (Free on MALI GPUs)
            EGL_SAMPLES, samples,

            // No idea what this does
            EGL_NONE
        };

        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

#if defined(PLATFORM_UWP)

        const EGLint surfaceAttributes[] = {
            // EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above).
            // If you have compilation issues with it then please update your Visual Studio templates.
            EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
            EGL_NONE
        };

        const EGLint defaultDisplayAttributes[] = {
            // These are the default display attributes, used to request ANGLE's D3D11 renderer.
            // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
            EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

            // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
            // Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
            EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,

            // EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call 
            // the IDXGIDevice3::Trim method on behalf of the application when it gets suspended. 
            // Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
            EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
            EGL_NONE,
        };

        const EGLint fl9_3DisplayAttributes[] = {
            // These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
            // These attributes are used if the call to eglInitialize fails with the default display attributes.
            EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
            EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
            EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
            EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
            EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
            EGL_NONE,
        };

        const EGLint warpDisplayAttributes[] = {
            // These attributes can be used to request D3D11 WARP.
            // They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
            EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
            EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
            EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
            EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
            EGL_NONE,
        };

        EGLConfig config = nullptr;

        // eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in Display attributes, used to configure D3D11.
        PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)(eglGetProcAddress("eglGetPlatformDisplayEXT"));
        if (!eglGetPlatformDisplayEXT) {
            throw std::runtime_error("Failed to get function eglGetPlatformDisplayEXT");
        }

        // To initialize the Display, we make three sets of calls to eglGetPlatformDisplayEXT and eglInitialize, with varying
        // parameters passed to eglGetPlatformDisplayEXT:
        // 1) The first calls uses "defaultDisplayAttributes" as a parameter. This corresponds to D3D11 Feature Level 10_0+.
        // 2) If eglInitialize fails for step 1 (e.g. because 10_0+ isn't supported by the default GPU), then we try again
        //    using "fl9_3DisplayAttributes". This corresponds to D3D11 Feature Level 9_3.
        // 3) If eglInitialize fails for step 2 (e.g. because 9_3+ isn't supported by the default GPU), then we try again
        //    using "warpDisplayAttributes".  This corresponds to D3D11 Feature Level 11_0 on WARP, a D3D11 software rasterizer.

        // This tries to initialize EGL to D3D11 Feature Level 10_0+. See above comment for details.
        Display = (void*)eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
        if (Display == EGL_NO_DISPLAY) {
            throw std::runtime_error("Failed to initialize EGL Display");
        }

        if (eglInitialize(Display, NULL, NULL) == EGL_FALSE) {
            // This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile devices).
            Display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
            if (Display == EGL_NO_DISPLAY) {
                throw std::runtime_error("Failed to initialize EGL Display");
            }

            if (eglInitialize(Display, NULL, NULL) == EGL_FALSE) {
                // This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
                Display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
                if (Display == EGL_NO_DISPLAY) {
                    throw std::runtime_error("Failed to initialize EGL Display");
                }

                if (eglInitialize(Display, NULL, NULL) == EGL_FALSE) {
                    // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
                    throw std::runtime_error("Failed to initialize EGL");
                }
            }
        }

        EGLint numConfigs = 0;
        if ((eglChooseConfig(Display, framebufferAttribs, &config, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0)) {
            throw std::runtime_error("Failed to choose first EGLConfig");
        }

#if defined(PLATFORM_UWP)
        // Create a PropertySet and initialize with the EGLNativeWindowType.
        PropertySet^ surfaceCreationProperties = ref new PropertySet();
        surfaceCreationProperties->Insert(ref new String(EGLNativeWindowTypeProperty), CoreWindow::GetForCurrentThread());     // CoreWindow^ window
#endif

        // You can configure the surface to render at a lower resolution and be scaled up to
        // the full window size. The scaling is often free on mobile hardware.
        //
        // One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
        // Size customRenderSurfaceSize = Size(800, 600);
        // surfaceCreationProperties->Insert(ref new String(EGLRenderSurfaceSizeProperty), PropertyValue::CreateSize(customRenderSurfaceSize));
        //
        // Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
        // e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
        // float customResolutionScale = 0.5f;
        // surfaceCreationProperties->Insert(ref new String(EGLRenderResolutionScaleProperty), PropertyValue::CreateSingle(customResolutionScale));


        // eglCreateWindowSurface() requires a EGLNativeWindowType parameter,
        // In Windows platform: typedef HWND EGLNativeWindowType;


        // Property: EGLNativeWindowTypeProperty
        // Type: IInspectable
        // Description: Set this property to specify the window type to use for creating a surface.
        //              If this property is missing, surface creation will fail.
        //
        //const wchar_t EGLNativeWindowTypeProperty[] = L"EGLNativeWindowTypeProperty";

        // https://stackoverflow.com/questions/46550182/how-to-create-eglsurface-using-c-winrt-and-angle

        // Surface = eglCreateWindowSurface(Display, config, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);
        Surface = eglCreateWindowSurface(Display, config, (EGLNativeWindowType)CoreWindow::GetForCurrentThread(), surfaceAttributes);
        
        if (Surface == EGL_NO_SURFACE) {
            throw std::runtime_error("Failed to create EGL fullscreen surface");
        }

        Context = eglCreateContext(Display, config, EGL_NO_CONTEXT, contextAttribs);
        if (Context == EGL_NO_CONTEXT) {
            throw std::runtime_error("Failed to create EGL context");
        }

#else
        // For future GLES2 platforms
        EGLint numConfigs;

        // Get an EGL display connection
        Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (Display == EGL_NO_DISPLAY) {
            throw std::runtime_error("Failed to initialize EGL display");
        }

        // Initialize the EGL display connection
        if (eglInitialize(Display, NULL, NULL) == EGL_FALSE) {
            // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
            throw std::runtime_error("Failed to initialize EGL");
        }

        // Get an appropriate EGL framebuffer configuration
        eglChooseConfig(Display, framebufferAttribs, &config, 1, &numConfigs);

        // Set rendering API
        eglBindAPI(EGL_OPENGL_ES_API);

        // Create an EGL rendering context
        Context = eglCreateContext(Display, config, EGL_NO_CONTEXT, contextAttribs);
        if (Context == EGL_NO_CONTEXT) {
            throw std::runtime_error("Failed to create EGL context");
        }
#endif

        // TODO: For other platforms, initialize window surface

        // Make context current
        if (eglMakeCurrent(Display, Surface, Surface, Context) == EGL_FALSE) {
            throw std::runtime_error("Unable to attach EGL rendering context to EGL surface");
        }

        // Get EGL Display window size
        eglQuerySurface(Display, Surface, EGL_WIDTH, &_CurrentWidth);
        eglQuerySurface(Display, Surface, EGL_HEIGHT, &_CurrentHeight);
#endif
        ConsoleMessage("Successfully created window.", "NOTICE", "Window");

#if defined(GRAPHICS_OPENGL21) || defined(GRAPHICS_OPENGL33) || defined(GRAPHICS_OPENGLES2)
        // Window is now created, init OpenGL
        Graphics::OpenGL::GL::Init();
        ConsoleMessage("The OpenGL API has been initialized.", "NOTICE", "Window");
#endif

        // Initialized
        _Initialized = true;

        // Apply any after-creation config
        ApplyConfig();
    }

    bool Window::IsFocussed() {
#if defined(PLATFORM_DESKTOP)
        return glfwGetWindowAttrib((GLFWwindow *)WindowPtr, GLFW_FOCUSED) == GLFW_TRUE;
#elif defined(PLATFORM_UWP)
        return CoreWindow::GetForCurrentThread()->ActivationMode == CoreWindowActivationMode::ActivatedInForeground;
#endif

        // Default to true
        return true;
    }

    void Window::PollEvents() {
#if defined(PLATFORM_DESKTOP)
        // Poll window events
        glfwPollEvents();

        // Query dimensions
        glfwGetWindowSize((GLFWwindow *)WindowPtr, &_CurrentWidth, &_CurrentHeight);
#elif defined(PLATFORM_UWP)
        // Poll window events
        if (Visible())
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
        else
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);

        // Query dimensions
        eglQuerySurface(Display, Surface, EGL_WIDTH, &_CurrentWidth);
        eglQuerySurface(Display, Surface, EGL_HEIGHT, &_CurrentHeight);
#endif
    }

    void Window::Resize(int width_, int height_) {
#if defined(PLATFORM_DESKTOP)
        // Set size
        glfwSetWindowSize((GLFWwindow *)WindowPtr, width_, height_);
#elif defined(PLATFORM_UWP)
        // Set size
        Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->TryResizeView(Windows::Foundation::Size(width_, height_));
#endif
    }

    void Window::SetConfig(const WindowConfig &config_) {
        Config = config_;
        ApplyConfig();
    }

    void Window::SetFullscreen(bool fullscreen_) {
        Config.Fullscreen = fullscreen_;
        ApplyConfig();
    }

    void Window::SetResizable(bool resizable_) {
        Config.Resizable = resizable_;
        ApplyConfig();
    }

    void Window::SetTitle(const std::string& title_) {
        Config.Title = title_;
        ApplyConfig();
    }

    void Window::SetVSync(bool vsync_) {
        Config.VSync = vsync_;
        ApplyConfig();
    }

    bool Window::ShouldClose() {
#if defined(PLATFORM_DESKTOP)
        return glfwWindowShouldClose((GLFWwindow *)WindowPtr);
#elif defined(PLATFORM_UWP)
        // UWP handles game closure differently. Do not close at any time due to window.
        return false;
#endif
        return true;
    }

    void Window::SwapBuffers() {
#if defined(PLATFORM_DESKTOP)
        glfwSwapBuffers((GLFWwindow *)WindowPtr);
#elif defined(PLATFORM_UWP)
        eglSwapBuffers(Display, Surface);
#endif
    }

    bool Window::Visible() {
#if defined(PLATFORM_DESKTOP)
        return glfwGetWindowAttrib((GLFWwindow *)WindowPtr, GLFW_ICONIFIED) == 0;
#elif defined(PLATFORM_UWP)
        return CoreWindow::GetForCurrentThread()->Visible == true;
#endif
        return true;
    }
}