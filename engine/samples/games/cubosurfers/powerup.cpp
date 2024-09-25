#include "powerup.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

static float inc = 0.0F;
static float timeBeforeInc = 3.0f;


CUBOS_REFLECT_IMPL(PowerUp)
{
    return cubos::core::ecs::TypeBuilder<PowerUp>("PowerUp")
        .withField("velocity", &PowerUp::velocity)
        .withField("killZ", &PowerUp::killZ)
        .build();
}

void reset()
{
    inc = 0.0F;
    timeBeforeInc = 3.0f;
}

void powerUpPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<PowerUp>();

    cubos.system("move powerups")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, const PowerUp&, Position&, Rotation&> powerups) {
            for (auto [ent, powerup, position, rotation] : powerups)
            {
                glm::vec3 new_vec = powerup.velocity - glm::vec3(0.0F, 0.0F, inc);
                position.vec += new_vec * dt.value();
                position.vec.y = glm::abs(glm::sin(position.vec.z * 0.025F)) * 1.5F;

                float rotationSpeed = glm::radians(180.0f); 
                glm::quat deltaRotation = glm::angleAxis(rotationSpeed * dt.value(), glm::vec3(0.0f, 1.0f, 0.0f));
                rotation.quat = deltaRotation * rotation.quat;
                rotation.quat = glm::normalize(rotation.quat);

                if (position.vec.z < powerup.killZ)
                {
                    cmds.destroy(ent);
                }
            }
            timeBeforeInc -= dt.value();
            if (timeBeforeInc <= 0)
            {
                inc += 0.5f;
                timeBeforeInc = 3.0f;
            }
            
        });
}

