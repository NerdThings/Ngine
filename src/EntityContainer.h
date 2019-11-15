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

#ifndef ENTITYCONTAINER_H
#define ENTITYCONTAINER_H

#include "Ngine.h"

// TODO: Needs rewritten.

namespace NerdThings::Ngine {
    // We do this forward declaration instead of include because BaseEntity uses this.
    class NEAPI BaseEntity;

    /*
     * An Entity Container, this just provides functions for entity management.
     * This is not normally used by a game.
     */
    class NEAPI EntityContainer {
        // Private Fields

        /*
         * The counter for unnamed entities
         */
        int _Counter = 0;

        /*
         * All of the entities in the scene.
         * Every entity has a name.
         */
        std::map<std::string, std::unique_ptr<BaseEntity>> _Entities;

        // Private Methods

        /*
         * Remove an entity parent
         */
        virtual void RemoveEntityParent(BaseEntity *ent_) = 0;

        /*
         * Set the entity parent
         */
        virtual void SetEntityParent(BaseEntity *ent_) = 0;
    public:
        // Destructors

        ~EntityContainer();

        // Public Methods

        /*
         * Add an entity without a name.
         * This will give the entity name a numerical value.
         * Returns pair of name and entity if success, null for both if fail
         */
        template <typename EntityType>
        std::pair<std::string, EntityType *> AddChild(EntityType *entity_) {
            // Cast to BaseEntity to ensure this is a valid type
            auto ent = dynamic_cast<BaseEntity*>(entity_);

            if (ent != nullptr) {
                std::string name = "Unique" + std::to_string(_Counter);
                _Entities.insert({name, std::unique_ptr<BaseEntity>(ent)});
                _Counter++;

                // Set parent
                SetEntityParent(ent);

                return std::make_pair(name, entity_);
            }

            return std::make_pair("", nullptr);
        }

        /*
         * Add an entity.
         * Returns entity if success, null if fail
         */
        template <typename EntityType>
        EntityType *AddChild(std::string name_, EntityType *entity_) {
            // Check the name is not taken
            if (HasChild(name_))
                return nullptr;

            // Cast to BaseEntity to ensure this is a valid type
            auto ent = dynamic_cast<BaseEntity*>(entity_);

            if (ent != nullptr) {
                _Entities.insert({name_, std::unique_ptr<BaseEntity>(ent)});

                // Set parent
                SetEntityParent(ent);

                return entity_;
            }

            return nullptr;
        }

        /*
         * Count all of the entities by type
         */
        template <typename EntityType = BaseEntity>
        int CountChildren() {
            int c = 0;
            for (auto e : GetEntities()) {
                if (dynamic_cast<EntityType*>(e) != nullptr) c++;
            }
            return c;
        }

        /*
         * Get all of the entities of type
         */
        template <typename EntityType = BaseEntity>
        std::vector<EntityType*> GetChildren() {
            std::vector<EntityType*> ents;
            for (auto it = _Entities.begin(); it != _Entities.end(); ++it) {
                auto ent = it->second.get();
                auto casted = dynamic_cast<EntityType*>(ent);
                if (casted != nullptr) ents.push_back(ent);
            }
            return ents;
        }

        /*
         * Get an entity by name.
         */
        template <typename EntityType = BaseEntity>
        EntityType *GetChild(const std::string &name_) {
            // Try to find the entity
            if (HasChild(name_)) {
                return dynamic_cast<EntityType*>(_Entities.at(name_).get()); // Will return null if its the wrong type
            }

            return nullptr;
        }

        /*
         * Test whether there is an entity by this name.
         */
        bool HasChild(const std::string &name_);

        /*
         * Remove an entity by name.
         * Returns success or fail.
         */
        bool RemoveChild(const std::string &name_);

        /*
         * Remove entity by pointer.
         */
        bool RemoveChild(BaseEntity *entity_);
    };
}

#endif //ENTITYCONTAINER_H
