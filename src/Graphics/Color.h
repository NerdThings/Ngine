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

#ifndef COLOR_H
#define COLOR_H

#include "../Ngine.h"

namespace NerdThings::Ngine::Graphics {
    /**
     * A 32-bit packed color.
     */
    struct NEAPI Color {
        /**
         * The packed value of the color.
         */
        unsigned int PackedValue;

        /**
         * Create the default color (White).
         */
        Color();

        /**
         * Create a color using an existing packed value.
         *
         * @param packedValue_ The packed value for the color.
         */
        explicit Color(const unsigned int packedValue_);

        /**
         * Create a color using float values between 0 and 1.
         *
         * @param r_ The red value.
         * @param g_ The green value.
         * @param b_ The blue value.
         * @param a_ The alpha (transparency) value.
         */
        Color(const float r_, const float g_, const float b_, const float a_);

        /**
         * Create a color using int values between 0 and 255.
         *
         * @param r_ The red value.
         * @param g_ The green value.
         * @param b_ The blue value.
         * @param a_ The alpha (transparency) value.
         */
        Color(const int r_, const int g_, const int b_, const int a_);

        /**
         * Get the red value as a float (0 - 1).
         */
        float GetRed() const;

        /**
         * Get the red value as an int (0-255).
         */
        int GetRedI() const;

        /**
         * Get the green value as a float (0 - 1).
         */
        float GetGreen() const;

        /**
         * Get the green value as an int (0-255).
         */
        int GetGreenI() const;

        /**
         * Get the blue value as a float (0 - 1).
         */
        float GetBlue() const;

        /**
         * Get the blue value as an int (0-255).
         */
        int GetBlueI() const;

        /**
         * Get the alpha value as a float (0 - 1).
         */
        float GetAlpha() const;

        /**
         * Get the alpha value as an int (0-255).
         */
        int GetAlphaI() const;

        bool operator==(const Color &b_) const;
        bool operator!=(const Color &b_) const;

        static const Color Beige;
        static const Color Black;
        static const Color Blue;
        static const Color Brown;
        static const Color DarkBlue;
        static const Color DarkBrown;
        static const Color DarkGray;
        static const Color DarkGreen;
        static const Color DarkPurple;
        static const Color Gold;
        static const Color Gray;
        static const Color Green;
        static const Color LightGray;
        static const Color Lime;
        static const Color Magenta;
        static const Color Orange;
        static const Color Pink;
        static const Color Purple;
        static const Color Red;
        static const Color SkyBlue;
        static const Color Transparent;
        static const Color Violet;
        static const Color White;
        static const Color Yellow;
    };
}

#endif //COLOR_H
