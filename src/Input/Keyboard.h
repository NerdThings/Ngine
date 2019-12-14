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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../Ngine.h"

#include "../Events.h"
#include "../Window.h"

#if defined(PLATFORM_DESKTOP)
struct GLFWwindow;
#endif

namespace NerdThings::Ngine::Input {
    /**
     * A keyboard key
     */
    enum Key {
        // For special purposes
        KEY_NONE = 0,

        // Alphanumeric keys
        KEY_APOSTROPHE = 39,
        KEY_COMMA = 44,
        KEY_MINUS = 45,
        KEY_PERIOD = 46,
        KEY_SLASH = 47,
        KEY_ZERO = 48,
        KEY_ONE = 49,
        KEY_TWO = 50,
        KEY_THREE = 51,
        KEY_FOUR = 52,
        KEY_FIVE = 53,
        KEY_SIX = 54,
        KEY_SEVEN = 55,
        KEY_EIGHT = 56,
        KEY_NINE = 57,
        KEY_SEMICOLON = 59,
        KEY_EQUAL = 61,
        KEY_A = 65,
        KEY_B = 66,
        KEY_C = 67,
        KEY_D = 68,
        KEY_E = 69,
        KEY_F = 70,
        KEY_G = 71,
        KEY_H = 72,
        KEY_I = 73,
        KEY_J = 74,
        KEY_K = 75,
        KEY_L = 76,
        KEY_M = 77,
        KEY_N = 78,
        KEY_O = 79,
        KEY_P = 80,
        KEY_Q = 81,
        KEY_R = 82,
        KEY_S = 83,
        KEY_T = 84,
        KEY_U = 85,
        KEY_V = 86,
        KEY_W = 87,
        KEY_X = 88,
        KEY_Y = 89,
        KEY_Z = 90,

        // Function keys
        KEY_SPACE = 32,
        KEY_ESCAPE = 256,
        KEY_ENTER = 257,
        KEY_TAB = 258,
        KEY_BACKSPACE = 259,
        KEY_INSERT = 260,
        KEY_DELETE = 261,
        KEY_RIGHT = 262,
        KEY_LEFT = 263,
        KEY_DOWN = 264,
        KEY_UP = 265,
        KEY_PAGE_UP = 266,
        KEY_PAGE_DOWN = 267,
        KEY_HOME = 268,
        KEY_END = 269,
        KEY_CAPS_LOCK = 280,
        KEY_SCROLL_LOCK = 281,
        KEY_NUM_LOCK = 282,
        KEY_PRINT_SCREEN = 283,
        KEY_PAUSE = 284,
        KEY_F1 = 290,
        KEY_F2 = 291,
        KEY_F3 = 292,
        KEY_F4 = 293,
        KEY_F5 = 294,
        KEY_F6 = 295,
        KEY_F7 = 296,
        KEY_F8 = 297,
        KEY_F9 = 298,
        KEY_F10 = 299,
        KEY_F11 = 300,
        KEY_F12 = 301,
        KEY_LEFT_SHIFT = 340,
        KEY_LEFT_CONTROL = 341,
        KEY_LEFT_ALT = 342,
        KEY_LEFT_SUPER = 343,
        KEY_RIGHT_SHIFT = 344,
        KEY_RIGHT_CONTROL = 345,
        KEY_RIGHT_ALT = 346,
        KEY_RIGHT_SUPER = 347,
        KEY_KB_MENU = 348,
        KEY_LEFT_BRACKET = 91,
        KEY_BACKSLASH = 92,
        KEY_RIGHT_BRACKET = 93,
        KEY_GRAVE = 96,

        // Keypad keys
        KEY_KP_0 = 320,
        KEY_KP_1 = 321,
        KEY_KP_2 = 322,
        KEY_KP_3 = 323,
        KEY_KP_4 = 324,
        KEY_KP_5 = 325,
        KEY_KP_6 = 326,
        KEY_KP_7 = 327,
        KEY_KP_8 = 328,
        KEY_KP_9 = 329,
        KEY_KP_DECIMAL = 330,
        KEY_KP_DIVIDE = 331,
        KEY_KP_MULTIPLY = 332,
        KEY_KP_SUBTRACT = 333,
        KEY_KP_ADD = 334,
        KEY_KP_ENTER = 335,
        KEY_KP_EQUAL = 336
    };

#define KEY_MAX 348

    /**
     * Key event args
     */
    struct KeyEventArgs : public EventArgs {
        /**
         * The key this event is fore
         */
        Input::Key Key;

        /**
         * Create a key event argument.
         *
         * @param key_ The key changed.
         */
        KeyEventArgs(Input::Key key_) : Key(key_) {}
    };

    /**
     * Keyboard input provider.
     *
     * @todo Need to have support for keymods.
     * @todo Allow resetting the state of keys (to prevent any actions until it is released, overriding down).
     */
    class NEAPI Keyboard {
        // Make window a friend so it can use our constructor
        friend class Ngine::Window;

        /**
         * Last frame's key state
         */
        bool m_previousKeyState[KEY_MAX + 1];

        /**
         * This frame's key state
         */
        bool m_currentKeyState[KEY_MAX + 1];

        /**
         * Next frame's key state.
         * This is here because inputs are handled on a separate thread to updates.
         */
        bool m_nextKeyState[KEY_MAX + 1];

        /**
         * The key used to exit the game.
         */
        Key m_exitKey;

        /**
         * The latest key pressed.
         */
        Key m_latestKeyPress;

#if defined(PLATFORM_DESKTOP)
        static void _GLFWKeyCallback(GLFWwindow *window_, int key_, int scancode_, int action_, int mods_);
#elif defined(PLATFORM_UWP)
        static Keyboard * m_UWPKeyboard;
        static Key KeyFromVirtualKey(int key_);
        static void UWPKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);
        static void UWPKeyUp(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);
#endif

        /**
         * Create a keyboard input manager.
         *
         * @param window_ The window.
         */
        Keyboard(Window *window_);
    public:
        /**
         * Key pressed.
         */
        Event<KeyEventArgs> KeyPressed;

        /**
         * Key released.
         */
        Event<KeyEventArgs> KeyReleased;

        /**
         * Get the current window's keyboard input manager.
         *
         * @return The current window's keyboard input manager.
         */
        static Keyboard *GetCurrent();

        /**
         * Get the latest keypress.
         *
         * @return The latest key that has been pressed.
         */
        Key GetLatestKeypress();

        /**
         * Is the key down.
         *
         * @param key_ Key to check
         * @return Whether the key is down or not.
         */
        bool IsKeyDown(Key key_);

        /**
         * Has a key been pressed this tick.
         *
         * @param key_ Key to check
         * @return Whether the key is was pressed this frame.
         */
        bool IsKeyPressed(Key key_);

        /**
         * Was the key released this frame.
         *
         * @param key_ Key to check
         * @return Whether the key was released this frame.
         */
        bool IsKeyReleased(Key key_);

        /**
         * Is the key up.
         *
         * @param key_ Key to check
         * @return Whether the key is up or not.
         */
        bool IsKeyUp(Key key_);

        /**
         * Poll keyboard inputs.
         */
        void PollInputs();

        /**
         * Set the key to close the game.
         *
         * @note This is most useful for debugging.
         * @todo Move this to Game??
         * @param key_ Set the key that can close the game.
         */
        void SetExitKey(Key key_);
    };
}

#endif //KEYBOARD_H
