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

#ifndef UISTYLE_H
#define UISTYLE_H

#include "../Ngine.h"

#include "../Graphics/Color.h"
#include "../Graphics/Texture2D.h"
#include "../Rectangle.h"

namespace NerdThings::Ngine::UI {
    /*
     * UI Styling information.
     * Some fields may not apply to some controls
     */
    struct NEAPI TUIStyle {
        // Public Fields

        /*
         * Background color
         */
        Graphics::Color BackColor = Graphics::Color::Transparent;

        /*
         * The background texture
         */
        Graphics::Texture2D *BackgroundTexture = nullptr;

        /*
         * Border color
         */
        Graphics::Color BorderColor = Graphics::Color::Gray;

        /*
         * Border thickness
         */
        float BorderThickness = 0;

        /*
         * Draw the default control geometry
         */
        bool DrawDefaults = true;

        /*
         * Foreground color
         */
        Graphics::Color ForeColor = Graphics::Color::Black;

        /*
         * Margins,
         * Up, Right, Down, Left
         */
        float Margin[4] = {0,0,0,0};

        /*
         * Padding,
         * Up, Right, Down, Left
         */
        float Padding[4] = {0, 0, 0, 0};

        // Public Methods

        /*
         * Must be passed the border position
         */
        Vector2 GetBackgroundPosition(Vector2 pos_) {
            // Apply border
            pos_ += BorderThickness;

            // Apply margin
            pos_.X += Margin[3];
            pos_.Y += Margin[0];

            return pos_;
        }

        Rectangle GetBackgroundRect(Rectangle controlRect_) {
            // Apply border
            controlRect_.X += BorderThickness;
            controlRect_.Y += BorderThickness;

            // Apply margin
            controlRect_.X += Margin[3];
            controlRect_.Y += Margin[0];

            // TODO: Do we need to take from width and height too???

            return controlRect_;
        }

        Graphics::Texture2D *GetBackgroundTexture() {
            return BackgroundTexture;
        }

        /*
         * Get the dimensions of the border
         */
        Vector2 GetBorderDimensions(Vector2 dimensions_) {
            // Apply border
            dimensions_ += BorderThickness * 2;

            return dimensions_;
        }

        /*
         * Get the border rectangle
         */
        Rectangle GetBorderRect(Rectangle controlRect_) {
            // Apply border
            controlRect_.Width += BorderThickness * 2;
            controlRect_.Height += BorderThickness * 2;

            // Apply margin TODO: Check this is right
            controlRect_.X += Margin[3];
            controlRect_.Y += Margin[0];

            return controlRect_;
        }

        /*
         * Get the position of the border
         */
        Vector2 GetBorderPosition(Vector2 pos_) {
            // Apply margin TODO: Check this is right
            pos_.X += Margin[3];
            pos_.Y += Margin[0];

            return pos_;
        }

        /*
         * The size of the content
         */
        Vector2 GetContentDimensions(Vector2 dimensions_) {
            // Apply padding
            dimensions_.X -= Padding[1];
            dimensions_.X -= Padding[3];
            dimensions_.Y -= Padding[2];
            dimensions_.Y -= Padding[0];

            return dimensions_;
        }

        /*
         * Get the start position for content inside the control.
         */
        Vector2 GetContentPosition(Vector2 pos_) {
            // Apply border
            pos_ += BorderThickness;

            // Apply padding
            pos_.X += Padding[3];
            pos_.Y += Padding[0];

            // Apply margin TODO: Check this is right
            pos_.X += Margin[3];
            pos_.Y += Margin[0];

            return pos_;
        }

        Rectangle GetContentRect(Rectangle controlRect_) {
            // Apply border
            controlRect_.X += BorderThickness;
            controlRect_.Y += BorderThickness;

            // Apply padding
            controlRect_.X += Padding[3];
            controlRect_.Y += Padding[0];
            controlRect_.Width -= Padding[1];
            controlRect_.Width -= Padding[3];
            controlRect_.Height -= Padding[2];
            controlRect_.Height -= Padding[0];

            // Apply margin TODO: Check this is right
            controlRect_.X += Margin[3];
            controlRect_.Y += Margin[0];

            return controlRect_;
        }

        /*
         * Set margin for all four values
         */
        void SetMargin(float all_) {
            for (auto i = 0; i < 4; i++)
                Margin[i] = all_;
        }

        /*
         * Set margin with a value for top and bottom and a value for the sides
         */
        void SetMargin(float ups_, float sides_) {
            Margin[0] = ups_;
            Margin[2] = ups_;
            Margin[1] = sides_;
            Margin[3] = sides_;
        }

        /*
         * Set margin values
         */
        void SetMargin(float up_, float right_, float down_, float left_) {
            Margin[0] = up_;
            Margin[1] = right_;
            Margin[2] = down_;
            Margin[3] = left_;
        }

        /*
         * Set padding for all four values
         */
        void SetPadding(float all_) {
            for (auto i = 0; i < 4; i++)
                Padding[i] = all_;
        }

        /*
         * Set padding with a value for top and bottom and a value for the sides
         */
        void SetPadding(float ups_, float sides_) {
            Padding[0] = ups_;
            Padding[2] = ups_;
            Padding[1] = sides_;
            Padding[3] = sides_;
        }

        /*
         * Set padding values
         */
        void SetPadding(float up_, float right_, float down_, float left_) {
            Padding[0] = up_;
            Padding[1] = right_;
            Padding[2] = down_;
            Padding[3] = left_;
        }
    };
}

#endif // UISTYLE_H
