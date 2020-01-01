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

#ifndef SPRITECOMPONENT_HPP
#define SPRITECOMPONENT_HPP

#include "../Config.hpp"

#include "../Graphics/Sprite.hpp"
#include "../Component.hpp"
#include "../Entity.hpp"
#include "../Events.hpp"

namespace NerdThings::Ngine::Components {
    /**
     * Sprite component. Attaches a sprite at the position of the entity.
     */
    class SpriteComponent : public Component {
        /**
         * RenderBatched origin.
         */
        Vector2 m_origin;

        /**
         * Sprite to render.
         */
        Graphics::Sprite m_sprite;
    public:
        /**
         * Create a sprite component.
         *
         * @param parent_ The parent entity we will attach to.
         * @param sprite_ The sprite to render with.
         * @param origin_ The sprite render origin.
         */
        SpriteComponent(Entity *parent_, const Graphics::Sprite &sprite_, Vector2 origin_ = Vector2::Zero);

        /**
         * Get the sprite render origin.
         *
         * @return The rendering origin.
         */
        Vector2 GetOrigin() const;

        /**
         * Set the sprite render origin.
         *
         * @param origin_ The new rendering origin.
         */
        void SetOrigin(const Vector2 &origin_);

        /**
         * Get a pointer to the sprite.
         *
         * @return The current sprite (pointer to it).
         */
        Graphics::Sprite *GetSprite();

        /**
         * Get a pointer to the sprite (read-only).
         *
         * @return A read-only pointer to the current sprite.
         */
        const Graphics::Sprite *GetSprite() const;

        /**
         * Set the sprite.
         *
         * @param sprite_ The new sprite to use.
         */
        void SetSprite(const Graphics::Sprite &sprite_);

        /**
         * Draw the sprite (called by the engine).
         */
        void Draw() override;

        /**
         * Update the sprite (called by the engine).
         */
        void Update() override;
    };
}

#endif //SPRITECOMPONENT_HPP
