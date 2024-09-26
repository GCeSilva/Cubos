#include "jetpack.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

extern float inc;

CUBOS_REFLECT_IMPL(Jetpack)
{
    return cubos::core::ecs::TypeBuilder<Jetpack>("Jetpack")
        .withField("velocity", &Jetpack::velocity)
        .withField("killZ", &Jetpack::killZ)
        .build();
}

void jetpackPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Jetpack>();

    cubos.system("move jetpacks")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, const Jetpack&, Position&, Rotation&> jetpacks) {
            for (auto [ent, jetpack, position, rotation] : jetpacks)
            {
                glm::vec3 new_vec = jetpack.velocity - glm::vec3(0.0F, 0.0F, inc);
                position.vec += new_vec * dt.value();
                position.vec.y = glm::abs(glm::sin(position.vec.z * 0.025F)) * 1.5F;

                float rotationSpeed = glm::radians(180.0f); 
                glm::quat deltaRotation = glm::angleAxis(rotationSpeed * dt.value(), glm::vec3(0.0f, 1.0f, 0.0f));
                rotation.quat = deltaRotation * rotation.quat;
                rotation.quat = glm::normalize(rotation.quat);

                if (position.vec.z < jetpack.killZ)
                {
                    cmds.destroy(ent);
                }
            }

        });
}

