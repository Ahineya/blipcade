// Include the quickjs.hpp header
#pragma once
#include "quickjs.hpp"
#include <sul/dynamic_bitset.hpp>

namespace blipcade::ecs {
    using Entity = std::uint32_t;
    using ComponentTypeID = std::size_t;
    using ComponentMask = sul::dynamic_bitset<>; // Use dynamic_bitset

    class ECS {
    public:
        // Constructor and Destructor
        ECS(JSContext* ctx);
        ~ECS();

        // Entity Management
        Entity createEntity();
        void destroyEntity(Entity entity);

        // Component Management
        void addComponent(Entity entity, const std::string& typeName, quickjs::value component);
        void removeComponent(Entity entity, const std::string& typeName);
        quickjs::value getComponent(Entity entity, const std::string& typeName);

        // Entity Iteration
        void forEachEntity(const std::vector<std::string> &componentTypes, const quickjs::value &callback, bool reverse);

        void processEntity(Entity entity, const ComponentMask &requiredMask,
                           const std::vector<ComponentTypeID> &typeIDs,
                           const quickjs::value &callback);

        bool isSubsetOf(const ComponentMask &requiredMask, const ComponentMask &entityMask);

    private:
        JSContext* ctx; // QuickJS context for managing JSValues

        // Component storage
        struct EntityData {
            ComponentMask componentMask;
            std::unordered_map<ComponentTypeID, quickjs::value> components;
        };

        std::vector<EntityData> entities;
        std::vector<Entity> freeEntities;
        std::vector<Entity> activeEntities;

        // Component type registry
        std::unordered_map<std::string, ComponentTypeID> componentTypeIDs;
        ComponentTypeID nextComponentTypeID = 0;

        // Helper methods
        ComponentTypeID getComponentTypeID(const std::string& typeName);
    };
}