#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/narrow_phase/colliding_with.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::CollidingWith)
{
    return core::ecs::TypeBuilder<CollidingWith>("cubos::engine::CollidingWith")
        .symmetric()
        .withField("entity", &CollidingWith::entity)
        .withField("other", &CollidingWith::other)
        .withField("penetration", &CollidingWith::penetration)
        .withField("position", &CollidingWith::position)
        .withField("normal", &CollidingWith::normal)
        .build();
}