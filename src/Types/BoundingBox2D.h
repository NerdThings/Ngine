/**********************************************************************************************
*
*   Ngine - A (mainly) 2D game engine.
*
*   Copyright (C) 2019 NerdThings
*
*   LICENSE: Apache License 2.0
*   View: https://github.com/NerdThings/Ngine/blob/master/LICENSE
*
**********************************************************************************************/

#ifndef BOUNDINGBOX2D_H
#define BOUNDINGBOX2D_H

#define TYPE_DECL_HEADER
#include "../ngine.h"
#undef TYPE_DECL_HEADER

#include "CollisionShape.h"
#include "Vector2.h"

namespace NerdThings::Ngine {
    /*
     * A 2D Bounding Box
     */
    struct NEAPI TBoundingBox2D : public ICollisionShape {
        // Public Fields

        /*
         * Maximum coordinate
         */
        TVector2 Max;

        /*
         * Minimum coordinate
         */
        TVector2 Min;

        // Public Constructor(s)

        /*
         * Create a default bounding box
         */
        TBoundingBox2D()
            : Max({}), Min({}) {}

        // Public Methods

        /*
         * Check for collision against other shapes
         */
        bool CheckCollision(ICollisionShape *shape_) override;
    };
}

#endif