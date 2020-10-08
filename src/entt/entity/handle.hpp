#ifndef ENTT_ENTITY_HANDLE_HPP
#define ENTT_ENTITY_HANDLE_HPP


#include "registry.hpp"


namespace entt {


/**
 * @brief Non-owning handle to an entity.
 *
 * Tiny wrapper around a registry and an entity.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
struct basic_handle {
    /*! @brief Underlying entity identifier. */
    using entity_type = std::remove_const_t<Entity>;
    /*! @brief Type of registry accepted by the handle. */
    using registry_type = std::conditional_t<std::is_const_v<Entity>, const basic_registry<entity_type>, basic_registry<entity_type>>;

    /*! @brief Constructs an invalid handle. */
    basic_handle() ENTT_NOEXCEPT
        : reg{}, entt{null}
    {}

    /**
     * @brief Constructs a handle from a given registry and entity.
     * @param ref An instance of the registry class.
     * @param value An entity identifier.
     */
    basic_handle(registry_type &ref, entity_type value) ENTT_NOEXCEPT
        : reg{&ref}, entt{value}
    {}

    /**
     * @brief Compares two handles.
     * @tparam Type A valid entity type (see entt_traits for more details).
     * @param other Handle with which to compare.
     * @return True if both handles refer to the same registry and the same
     * entity, false otherwise.
     */
    template<typename Type>
    [[nodiscard]] bool operator==(const basic_handle<Type> &other) const ENTT_NOEXCEPT {
        return reg == other.registry() && entt == other.entity();
    }

    /**
     * @brief Constructs a const handle from a non-const one.
     * @return A const handle referring to the same registry and the same
     * entity.
     */
    [[nodiscard]] operator basic_handle<const entity_type>() const ENTT_NOEXCEPT {
        return reg ? basic_handle<const entity_type>{*reg, entt} : basic_handle<const entity_type>{};
    }

    /**
     * @brief Converts a handle to its underlying entity.
     * @return An entity identifier.
     */
    [[nodiscard]] operator entity_type() const ENTT_NOEXCEPT {
        return entity();
    }

    /**
     * @brief Checks if a handle refers to non-null registry pointer and entity.
     * @return True if the handle refers to non-null registry and entity, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return reg && entt != null;
    }

    /**
     * @brief Checks if a handle refers to a valid entity or not.
     * @return True if the handle refers to a valid entity, false otherwise.
     */
    [[nodiscard]] bool valid() const {
        return reg->valid(entt);
    }

    /**
     * @brief Returns a pointer to the underlying registry, if any.
     * @return A pointer to the underlying registry, if any.
     */
    [[nodiscard]] registry_type * registry() const ENTT_NOEXCEPT {
        return reg;
    }

    /**
     * @brief Returns the entity associated with a handle.
     * @return The entity associated with the handle.
     */
    [[nodiscard]] entity_type entity() const ENTT_NOEXCEPT {
        return entt;
    }

    /**
     * @brief Assigns the given component to a handle.
     * @sa basic_registry::emplace
     * @tparam Component Type of component to create.
     * @tparam Args Types of arguments to use to construct the component.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the newly created component.
     */
    template<typename Component, typename... Args>
    decltype(auto) emplace(Args &&... args) const {
        return reg->template emplace<Component>(entt, std::forward<Args>(args)...);
    }

    /**
     * @brief Assigns or replaces the given component for a handle.
     * @sa basic_registry::emplace_or_replace
     * @tparam Component Type of component to assign or replace.
     * @tparam Args Types of arguments to use to construct the component.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the newly created component.
     */
    template<typename Component, typename... Args>
    decltype(auto) emplace_or_replace(Args &&... args) const {
        return reg->template emplace_or_replace<Component>(entt, std::forward<Args>(args)...);
    }

    /**
     * @brief Patches the given component for a handle.
     * @sa basic_registry::patch
     * @tparam Component Type of component to patch.
     * @tparam Func Types of the function objects to invoke.
     * @param func Valid function objects.
     * @return A reference to the patched component.
     */
    template<typename Component, typename... Func>
    decltype(auto) patch(Func &&... func) const {
        return reg->template patch<Component>(entt, std::forward<Func>(func)...);
    }

    /**
     * @brief Replaces the given component for a handle.
     * @sa basic_registry::replace
     * @tparam Component Type of component to replace.
     * @tparam Args Types of arguments to use to construct the component.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the component being replaced.
     */
    template<typename Component, typename... Args>
    decltype(auto) replace(Args &&... args) const {
        return reg->template replace<Component>(entt, std::forward<Args>(args)...);
    }

    /**
     * @brief Removes the given components from a handle.
     * @sa basic_registry::remove
     * @tparam Component Types of components to remove.
     */
    template<typename... Component>
    void remove() const {
        reg->template remove<Component...>(entt);
    }

    /**
     * @brief Removes the given components from a handle.
     * @sa basic_registry::remove_if_exists
     * @tparam Component Types of components to remove.
     * @return The number of components actually removed.
     */
    template<typename... Component>
    decltype(auto) remove_if_exists() const {
        return reg->template remove_if_exists<Component...>(entt);
    }

    /**
     * @brief Removes all the components from a handle and makes it orphaned.
     * @sa basic_registry::remove_all
     */
    void remove_all() const {
        reg->remove_all(entt);
    }

    /**
     * @brief Checks if a handle has all the given components.
     * @sa basic_registry::has
     * @tparam Component Components for which to perform the check.
     * @return True if the handle has all the components, false otherwise.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) has() const {
        return reg->template has<Component...>(entt);
    }

    /**
     * @brief Checks if a handle has at least one of the given components.
     * @sa basic_registry::any
     * @tparam Component Components for which to perform the check.
     * @return True if the handle has at least one of the given components,
     * false otherwise.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) any() const {
        return reg->template any<Component...>(entt);
    }

    /**
     * @brief Returns references to the given components for a handle.
     * @sa basic_registry::get
     * @tparam Component Types of components to get.
     * @return References to the components owned by the handle.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) get() const {
        return reg->template get<Component...>(entt);
    }

    /**
     * @brief Returns a reference to the given component for a handle.
     * @sa basic_registry::get_or_emplace
     * @tparam Component Type of component to get.
     * @tparam Args Types of arguments to use to construct the component.
     * @param args Parameters to use to initialize the component.
     * @return Reference to the component owned by the handle.
     */
    template<typename Component, typename... Args>
    [[nodiscard]] decltype(auto) get_or_emplace(Args &&... args) const {
        return reg->template get_or_emplace<Component>(entt, std::forward<Args>(args)...);
    }

    /**
     * @brief Returns pointers to the given components for a handle.
     * @sa basic_registry::try_get
     * @tparam Component Types of components to get.
     * @return Pointers to the components owned by the handle.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) try_get() const {
        return reg->template try_get<Component...>(entt);
    }

    /**
     * @brief Checks if a handle has components assigned.
     * @return True if the handle has no components assigned, false otherwise.
     */
    [[nodiscard]] bool orphan() const {
        return reg->orphan(entt);
    }

    /**
     * @brief Visits a handle and returns the types for its components.
     * @sa basic_registry::visit
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void visit(Func &&func) const {
        reg->visit(entt, std::forward<Func>(func));
    }

private:
    registry_type *reg;
    entity_type entt;
};


/**
 * @brief Compares two handles.
 * @tparam Type A valid entity type (see entt_traits for more details).
 * @tparam Other A valid entity type (see entt_traits for more details).
 * @param lhs A valid handle.
 * @param rhs A valid handle.
 * @return False if both handles refer to the same registry and the same
 * entity, true otherwise.
 */
template<typename Type, typename Other>
bool operator!=(const basic_handle<Type> &lhs, const basic_handle<Other> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Deduction guide.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
basic_handle(basic_registry<Entity> &, Entity) -> basic_handle<Entity>;


/**
 * @brief Deduction guide.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
basic_handle(const basic_registry<Entity> &, Entity) -> basic_handle<const Entity>;


}


#endif