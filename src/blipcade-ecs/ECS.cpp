#include "ECS.h"
#include <cassert>
#include <iostream>

namespace blipcade::ecs {
    ECS::ECS(JSContext *ctx) : ctx(ctx), entities({}), freeEntities({}), componentTypeIDs({}) {
    }

    ECS::~ECS() {
        // Components are automatically cleaned up by quickjs::value's destructor
        for (auto &entityData: entities) {
            entityData.components.clear();
            entityData.componentMask.clear(); // Clear the bitset
        }
    }

    Entity ECS::createEntity() {
        Entity entity;
        if (!freeEntities.empty()) {
            entity = freeEntities.back();
            freeEntities.pop_back();
            entities[entity].componentMask.clear(); // Reset the dynamic_bitset
            entities[entity].components.clear();
        } else {
            entity = static_cast<Entity>(entities.size());
            entities.emplace_back();
        }
        return entity;
    }

    void ECS::destroyEntity(Entity entity) {
        assert(entity < entities.size() && "Entity out of range.");

        // Components will be cleaned up automatically

        // something here causes a sigtrap
        entities[entity].components.clear();
        entities[entity].componentMask.clear();

        freeEntities.push_back(entity);
    }

    ComponentTypeID ECS::getComponentTypeID(const std::string &typeName) {
        auto it = componentTypeIDs.find(typeName);
        if (it == componentTypeIDs.end()) {
            // Assign a new ID
            ComponentTypeID id = nextComponentTypeID++;
            componentTypeIDs[typeName] = id;
            return id;
        }
        return it->second;
    }

    void ECS::addComponent(Entity entity, const std::string &typeName, quickjs::value component) {
        assert(entity < entities.size() && "Entity out of range.");

        ComponentTypeID typeID = getComponentTypeID(typeName);

        auto &components = entities[entity].components;

        // Assign the component; old value will be automatically cleaned up
        components[typeID] = std::move(component);

        // Update the component mask
        if (typeID >= entities[entity].componentMask.size()) {
            entities[entity].componentMask.resize(typeID + 1, false);
        }
        entities[entity].componentMask.set(typeID);
    }

    void ECS::removeComponent(Entity entity, const std::string &typeName) {
        assert(entity < entities.size() && "Entity out of range.");

        ComponentTypeID typeID = getComponentTypeID(typeName);

        auto &components = entities[entity].components;
        components.erase(typeID);

        // Update the component mask
        if (typeID < entities[entity].componentMask.size()) {
            entities[entity].componentMask.reset(typeID);
        }
    }

    quickjs::value ECS::getComponent(Entity entity, const std::string &typeName) {
        assert(entity < entities.size() && "Entity out of range.");

        ComponentTypeID typeID = getComponentTypeID(typeName);

        auto &components = entities[entity].components;
        auto it = components.find(typeID);
        if (it != components.end()) {
            // Return a copy; quickjs::value handles reference counting
            return it->second;
        }
        return quickjs::value::undefined(ctx);
    }

    void ECS::forEachEntity(const std::vector<std::string> &componentTypes, const quickjs::value &callback, bool reverse = false) {
        // Ensure the callback is a function
        if (!callback.is_function()) {
            throw quickjs::throw_exception(
                quickjs::value::type_error(callback.get_context(), "Callback must be a function")
            );
        }

        // Build the component mask for the required components
        ComponentMask requiredMask;
        std::vector<ComponentTypeID> typeIDs;
        for (const auto &typeName: componentTypes) {
            ComponentTypeID typeID = getComponentTypeID(typeName);
            typeIDs.push_back(typeID);
            if (typeID >= requiredMask.size()) {
                requiredMask.resize(typeID + 1, false);
            }
            requiredMask.set(typeID);
        }

        for (Entity entity = 0; entity < entities.size(); ++entity) {
            const auto &entityData = entities[entity];

            // Ensure the entity's component mask is large enough
            if (entityData.componentMask.size() < requiredMask.size()) {
                continue;
            }

            // Check if the entity has all the required components using bitmask
            if (isSubsetOf(requiredMask, entityData.componentMask)) {
                // Prepare arguments: entity ID and component values
                size_t argc = 1 + typeIDs.size();
                std::vector<quickjs::value> argv;
                argv.reserve(argc);

                // First argument: entity ID as a number
                argv.emplace_back(callback.get_context(), entity);

                // Subsequent arguments: component values
                for (size_t i = 0; i < typeIDs.size(); ++i) {
                    ComponentTypeID typeID = typeIDs[i];
                    argv.push_back(entityData.components.at(typeID));
                }

                // Call the callback function
                try {
                    callback.call(quickjs::value::undefined(callback.get_context()), argv.begin(), argv.end());
                } catch (const quickjs::value_error &e) {
                    std::cerr << "JavaScript error: " << e.what() << std::endl;
                    if (e.stack()) {
                        std::cerr << "Stack trace: " << e.stack() << std::endl;
                    }
                } catch (const quickjs::value_exception &e) {
                    auto val = e.val();
                    if (val.valid()) {
                        std::cerr << "JavaScript exception: " << val.as_cstring().c_str() << std::endl;

                        // Try to get the stack trace
                        auto stack = val.get_property("stack");
                        if (stack.valid() && !stack.is_undefined()) {
                            std::cerr << "Stack trace:\n" << stack.as_cstring().c_str() << std::endl;
                        } else {
                            std::cerr << "No stack trace available." << std::endl;
                        }
                    } else {
                        std::cerr << "Invalid JavaScript exception value" << std::endl;
                    }
                } catch (const quickjs::value_error &e) {
                    std::cerr << "JavaScript error: " << e.what() << std::endl;
                    if (e.stack()) {
                        std::cerr << "Stack trace:\n" << e.stack() << std::endl;
                    }
                } catch (const quickjs::exception &e) {
                    std::cerr << "QuickJS exception: " << e.what() << std::endl;
                } catch (const std::exception &e) {
                    std::cerr << "C++ exception: " << e.what() << std::endl;
                } catch (...) {
                    std::cerr << "Unknown exception" << std::endl;
                }
            }
        }
    }

    bool ECS::isSubsetOf(const ComponentMask &requiredMask, const ComponentMask &entityMask) {
        if (requiredMask.size() > entityMask.size()) {
            // If requiredMask has bits set beyond entityMask's size, it cannot be a subset
            return false;
        }

        // Iterate over the bits in requiredMask
        for (size_t i = 0; i < requiredMask.size(); ++i) {
            if (requiredMask.test(i) && !entityMask.test(i)) {
                return false; // Found a required component that the entity doesn't have
            }
        }
        return true; // All required components are present in the entity
    }
}
