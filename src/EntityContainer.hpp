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

#ifndef ENTITYCONTAINER_HPP
#define ENTITYCONTAINER_HPP

#include "Config.hpp"

namespace NerdThings::Ngine {
    // We do this forward declaration instead of include because Entity uses this.
    class Entity;

    /**
     * This is a wrapper class which helps with having child entities.
     */
    class NEAPI EntityContainer {
    protected:
        /**
         * A container type. Used for running actions on children.
         */
        enum ContainerType {
            ENTITY = 0,
            SCENE
        };
    private:
        /**
         * The container type.
         */
        ContainerType m_type = ENTITY;

        /**
         * The counter for unnamed entities.
         */
        int m_counter = 0;

        /**
         * All of the entities in the scene.
         * Every entity has a name.
         */
        std::map<std::string, Entity *> m_entities;

        /**
         * Register entity in its parent.
         */
        void _addEntity(Entity *entity_);
    protected:
        /**
         * Initialize the entity container.
         *
         * @param type_ The container type.
         */
        explicit EntityContainer(ContainerType type_);
    public:
        ~EntityContainer();

        /**
         * Add an unnamed entity.
         *
         * @note The entity's lifecycle is now managed by this container.
         * @tparam EntityType The entity type. Must be derrived from NerdThings::Ngine::Entity.
         * @param entity_ The entity to add.
         * @return An std::pair with first being name and second the entity
         */
        template <class EntityType = Entity>
        std::pair<std::string, EntityType *> AddChild(EntityType *entity_) {
            // Get as base type
            auto ent = (Entity *) entity_;

            if (ent != nullptr) {
                std::string name = "Unique" + std::to_string(m_counter);
                m_entities.insert({name, ent});
                m_counter++;

                // Setup entity
                _addEntity(ent);

                return std::make_pair(name, entity_);
            }

            return std::make_pair("", nullptr);
        }

        /**
         * Add an entity.
         *
         * @note The entity's lifecycle is now managed by this container.
         * @tparam EntityType The entity type. Must be derrived from NerdThings::Ngine::Entity.
         * @param name_ The name of the entity.
         * @param entity_ The entity to add.
         * @return The entity, for chaining commands.
         */
        template <class EntityType = Entity>
        EntityType *AddChild(std::string name_, EntityType *entity_) {
            // Check the name is not taken
            if (HasChild(name_))
                return nullptr;

            // Get as base type
            auto ent = (Entity *) entity_;

            if (ent != nullptr) {
                m_entities.insert({name_, ent});

                // Setup entity
                _addEntity(ent);

                return entity_;
            }

            return nullptr;
        }

        /**
         * Remove an entity by name.
         *
         * @param name_ The name of the entity to remove.
         * @param Whether or not to delete the entity from memory.
         * @return Whether the entity was removed or not.
         */
        bool RemoveChild(const std::string &name_, bool delete_ = true);

        /**
         * Remove entity by pointer.
         *
         * @param entity_ The entity to be removed.
         * @param Whether or not to delete the entity from memory.
         * @return Whether the entity was removed or not.
         */
        bool RemoveChild(Entity *entity_, bool delete_ = true);

        /**
         * Get an entity by name.
         *
         * @tparam EntityType The entity type. Must be derrived from NerdThings::Ngine::Entity.
         * @param name_ The name of the entity to get.
         * @return The entity, or null if not found.
         */
        template <class EntityType = Entity>
        EntityType *GetChild(const std::string &name_) {
            // Try to find the entity
            if (HasChild(name_)) {
                return (EntityType *) m_entities.at(name_);
            }

            return nullptr;
        }

        /**
         * Count all of the entities by type
         *
         * @tparam EntityType The entity type. Must be derrived from NerdThings::Ngine::Entity.
         * @return The number of children who match the selected type.
         */
        template <class EntityType = Entity>
        int CountChildren() {
            int c = 0;
            for (auto e : GetChildren()) {
                // Dynamic cast so that we count only the correct children.
                // TODO: Way to identify entity types without dynamically casting. (Reflection maybe?)
                if (dynamic_cast<EntityType*>(e) != nullptr) c++;
            }
            return c;
        }

        /**
         * Get all of the entities of type.
         *
         * @tparam EntityType The entity type. Must be derrived from NerdThings::Ngine::Entity.
         * @return A vector with all of the entities of the provided type.
         */
        template <class EntityType = Entity>
        std::vector<EntityType*> GetChildren() {
            std::vector<EntityType*> ents;
            for (auto it = m_entities.begin(); it != m_entities.end(); ++it) {
                auto ent = it->second;

                // Dynamic cast to check again TODO find a different way.
                auto casted = dynamic_cast<EntityType*>(ent);
                if (casted != nullptr) ents.push_back(ent);
            }
            return ents;
        }

        /**
         * Test whether there is an entity by this name.
         *
         * @param name_ The entity to test for.
         * @return Whether the entity exists or not.
         */
        template<class EntityType = Entity>
        bool HasChild(const std::string &name_) {
            auto exists = m_entities.find(name_) != m_entities.end();
            if (!exists) return false;

            // Dynamic cast to check type.
            return dynamic_cast<EntityType *>(m_entities[name_]) != nullptr;
        }
    };
}

#endif //ENTITYCONTAINER_HPP