#pragma once
#include "quickjs.hpp"
#include <sul/dynamic_bitset.hpp>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <nlohmann/json_fwd.hpp>

namespace blipcade::ecs {
    using Entity = std::uint32_t;
    using ComponentTypeID = std::size_t;
    using ComponentMask = sul::dynamic_bitset<>; // Use dynamic_bitset
    using ComponentProperty = std::pair<std::string, quickjs::value>;

    class ECS {
    public:
        // Constructor and Destructor
        ECS(quickjs::context &ctx);

        ~ECS();


        // Entity Management
        Entity createEntity();

        void destroyEntity(Entity entity);

        // Component Management
        void addComponent(Entity entity, const std::string &typeName, quickjs::value component);

        void removeComponent(Entity entity, const std::string &typeName);

        quickjs::value getComponent(Entity entity, const std::string &typeName);

        void processEntities(const quickjs::value &callback, const ComponentMask &requiredMask,
                             const std::vector<ComponentTypeID> &typeIDs,
                             Entity entity);

        // Entity Iteration
        void forEachEntity(const std::vector<std::string> &componentTypes, const quickjs::value &callback,
                           bool reverse);

        bool isSubsetOf(const ComponentMask &requiredMask, const ComponentMask &entityMask);

        nlohmann::json serializeECS();
        nlohmann::json serializeEntity(Entity entity);

        nlohmann::json quickjsValueToJson(const quickjs::value &val);

        // Devtool methods
        const std::vector<Entity> &getActiveEntities() const;

        const std::unordered_map<std::string, ComponentTypeID> &getComponentTypeIDs() const;

        std::unordered_map<ComponentTypeID, quickjs::value> &getComponents(Entity entity);

        std::string getComponentName(ComponentTypeID typeID);

        const std::unordered_map<ComponentTypeID, quickjs::value> &getComponents(Entity entity) const;

        // Additional helper methods
        quickjs::context &getContext() { return ctx; }

        std::vector<ComponentProperty> getComponentProperties(quickjs::value &component);

    private:
        quickjs::context &ctx; // QuickJS context for managing JSValues

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

        // Deferred operations
        size_t iterationDepth = 0; // Changed from bool to size_t
        std::vector<Entity> pendingRemovals;
        std::vector<Entity> pendingAdditions;

        // Helper methods
        ComponentTypeID getComponentTypeID(const std::string &typeName);

        const std::string getComponentTypeName(ComponentTypeID typeID);

        void applyDeferredOperations();
    };
}
