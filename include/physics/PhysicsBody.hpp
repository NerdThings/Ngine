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

#ifndef PHYSICSBODY_HPP
#define PHYSICSBODY_HPP

#include "../Config.hpp"

#include "../Math.hpp"
#include "PhysicsFixture.hpp"

// Forward declaring Box2D
class b2Body;

namespace ngine::physics {
    class PhysicsWorld;

    enum class PhysicsBodyType {
        Static,
        Kinematic,
        Dynamic
    };

    class NEAPI PhysicsBody final {
        friend class PhysicsWorld;

        /**
         * The Box2D body.
         */
        b2Body *m_body = nullptr;

        /**
         * Our version of the physics world.
         */
        PhysicsWorld *m_world = nullptr;

        /**
         * List of fixtures.
         * This is soley for memory management when Box2D forgets us...
         */
        std::vector<PhysicsFixture *> m_fixtures;

        /**
         * Create a physics body.
         */
        PhysicsBody(PhysicsWorld *world, b2Body *body);

        /**
         * Destroy fixtures.
         */
        void _destroy();
    public:

        // PhysicsFixture *createFixture();

        void destroyFixture(PhysicsFixture *fixture);

        /**
         * Get the attached physics world.
         */
        PhysicsWorld *getWorld();

        Vector2 getPosition();

        void setPosition(Vector2 position);

        Angle getRotation();

        void setRotation(Angle rotation);

        Transform2D getTransform();

        void setTransform(Transform2D transform);

        Vector2 getWorldCenter();

        Vector2 getLocalCenter();

        Vector2 getLinearVelocity();

        void setLinearVelocity(Vector2 velocity);

        float getAngularVelocity();

        void setAngularVelocity(float velocity);

        float getMass();

        /**
         * Get inertia in kgpixels^2.
         */
        float getInertia();

        void resetMassData();

        PhysicsBodyType getType();

        void setType(PhysicsBodyType type);
    };
}

#endif //PHYSICSBODY_HPP
