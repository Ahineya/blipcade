#include "ECS.h"
#include <cassert>
#include <iostream>
#include <quickjs.hpp>
#include <nlohmann/json.hpp>
using namespace quickjs;

namespace blipcade::ecs {



    ECS::ECS(quickjs::context &ctx) : ctx(ctx), entities({}), freeEntities({}), componentTypeIDs({}), activeEntities({}) {
    }

    ECS::~ECS() {
        // Components are automatically cleaned up by quickjs::value's destructor
        for (auto &entityData: entities) {
            entityData.components.clear();
            entityData.componentMask.clear(); // Clear the bitset
        }
    }

    std::vector<ComponentProperty> ECS::getComponentProperties(value& component) {
        std::vector<ComponentProperty> properties;

        if (!component.is_object()) {
            return properties;
        }

        // Get the Object global
        value Object = ctx.get_global_object().get_property("Object");
        // Get the keys of the object
        value keys = Object.call_member("keys", component);

        if (!keys.is_array()) {
            return properties;
        }

        uint32_t length = keys.get_property("length").as_uint32();

        for (uint32_t i = 0; i < length; ++i) {
            quickjs::value key = keys.get_property(i);
            if (key.is_string()) {
                std::string keyStr = key.as_cstring().c_str();
                quickjs::value value = component.get_property(keyStr.c_str());
                properties.emplace_back(keyStr, value);
            }
        }

        return properties;
    }

    Entity ECS::createEntity() {
        Entity entity;
        if (!freeEntities.empty()) {
            entity = freeEntities.back();
            freeEntities.pop_back();
            entities[entity].componentMask.reset();
            entities[entity].components.clear();
        } else {
            entity = static_cast<Entity>(entities.size());
            entities.emplace_back();
        }

        if (iterationDepth > 0) {
            pendingAdditions.push_back(entity);
        } else {
            activeEntities.push_back(entity);
        }

        return entity;
    }

    void ECS::destroyEntity(Entity entity) {
        assert(entity < entities.size() && "Entity out of range.");

        if (iterationDepth > 0) {
            pendingRemovals.push_back(entity);
        } else {
            // Remove the entity from the active list
            auto it = std::find(activeEntities.begin(), activeEntities.end(), entity);
            if (it != activeEntities.end()) {
                activeEntities.erase(it);
            }

            // Components will be cleaned up automatically
            entities[entity].components.clear();
            entities[entity].componentMask.reset();

            freeEntities.push_back(entity);
        }
    }

    void ECS::applyDeferredOperations() {
        // Process pending removals
        for (Entity entity: pendingRemovals) {
            // Remove the entity from the active list
            auto it = std::find(activeEntities.begin(), activeEntities.end(), entity);
            if (it != activeEntities.end()) {
                activeEntities.erase(it);
            }

            // Components will be cleaned up automatically
            entities[entity].components.clear();
            entities[entity].componentMask.reset();

            freeEntities.push_back(entity);
        }
        pendingRemovals.clear();

        // Process pending additions
        for (Entity entity: pendingAdditions) {
            activeEntities.push_back(entity);
        }
        pendingAdditions.clear();
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

    const std::string ECS::getComponentTypeName(ComponentTypeID typeID) {
        for (const auto &pair: componentTypeIDs) {
            if (pair.second == typeID) {
                return pair.first;
            }
        }
        return "";
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

    void ECS::processEntities(const quickjs::value &callback, const ComponentMask& requiredMask, const std::vector<ComponentTypeID> &typeIDs, Entity entity) {
        if (entity >= entities.size()) {
            std::cerr << "Invalid entity ID detected: " << entity << std::endl;
            return;
        }

        const auto &entityData = entities[entity];

        // Ensure the entity's component mask is large enough
        if (entityData.componentMask.size() < requiredMask.size()) {
            return;
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

    void ECS::forEachEntity(const std::vector<std::string> &componentTypes, const quickjs::value &callback,
                            bool reverse) {
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

        iterationDepth++;

        if (reverse) {
            for (auto it = activeEntities.rbegin(); it != activeEntities.rend(); ++it) {
                processEntities(callback, requiredMask, typeIDs, *it);
            }
        } else {
            for (Entity entity: activeEntities) {
                processEntities(callback, requiredMask, typeIDs, entity);
            }
        }
        // for (Entity entity: activeEntities) {
            // processEntities(callback, requiredMask, typeIDs, entity);
        // }

        iterationDepth--;
        assert(iterationDepth >= 0 && "Iteration depth cannot be negative.");
        if (iterationDepth == 0) {
            applyDeferredOperations();
        }
    }

    const std::vector<Entity>& ECS::getActiveEntities() const {
        return activeEntities;
    }

    const std::unordered_map<std::string, ComponentTypeID>& ECS::getComponentTypeIDs() const {
        return componentTypeIDs;
    }

    std::unordered_map<ComponentTypeID, quickjs::value>& ECS::getComponents(Entity entity) {
        assert(entity < entities.size() && "Entity out of range.");
        return entities[entity].components;
    }

    std::string ECS::getComponentName(ComponentTypeID typeID) {
        for (const auto &pair: componentTypeIDs) {
            if (pair.second == typeID) {
                return pair.first;
            }
        }
        return "";
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

    nlohmann::json ECS::serializeECS() {
        nlohmann::json json;

        // Serialize component type IDs
        for (const auto &pair: componentTypeIDs) {
            json["componentTypeIDs"][pair.first] = pair.second;
        }

        // Serialize entities
        for (Entity entity: activeEntities) {
            json["entities"][entity] = serializeEntity(entity);
        }

        return json;
    }

    nlohmann::json ECS::serializeEntity(Entity entity) {
        nlohmann::json json;

        const auto &entityData = entities[entity];

        // Serialize component mask
        json["componentMask"] = entityData.componentMask.to_string();

        // Serialize components
        for (const auto &pair: entityData.components) {
            const std::string &typeName = getComponentName(pair.first);
            auto serialized = quickjsValueToJson(pair.second);

            std::cout << "Serialized: " << serialized.dump() << std::endl;
            json["components"][typeName] = serialized;
        }

        return json;
    }

    nlohmann::json ECS::quickjsValueToJson(const quickjs::value &val) {
        if (val.is_null() || val.is_undefined()) {
            return nullptr;
        } else if (val.is_bool()) {
            return val.as_bool();
        } else if (val.is_number()) {
            return val.as_double();
        } else if (val.is_string()) {
            return val.as_cstring();
        } else if (val.is_array()) {
            nlohmann::json arr = nlohmann::json::array();
            uint32_t length = val.get_property("length").as_uint32();
            for (uint32_t i = 0; i < length; ++i) {
                quickjs::value elem = val.get_property(i);
                arr.push_back(quickjsValueToJson(elem));
            }
            return arr;
        } else if (val.is_object()) {
            nlohmann::json obj = nlohmann::json::object();
            // Get property names
            quickjs::value Object = val.get_context().get_global_object().get_property("Object");
            quickjs::value keys = Object.call_member("keys", val);
            if (!keys.is_array()) {
                return obj;
            }
            uint32_t length = keys.get_property("length").as_uint32();
            for (uint32_t i = 0; i < length; ++i) {
                quickjs::value key = keys.get_property(i);
                if (key.is_string()) {
                    std::string keyStr = key.as_cstring();
                    quickjs::value propValue = val.get_property(keyStr.c_str());
                    obj[keyStr] = quickjsValueToJson(propValue);
                }
            }
            return obj;
        } else {
            // Other types (e.g., functions, symbols) are not serializable to JSON
            return nullptr;
        }
    }
}
