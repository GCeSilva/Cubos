#include "obstacle.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

static float inc = 0.0F;
static float timeBeforeInc = 3.0f;


CUBOS_REFLECT_IMPL(Obstacle)
{
    return cubos::core::ecs::TypeBuilder<Obstacle>("Obstacle")
        .withField("velocity", &Obstacle::velocity)
        .withField("killZ", &Obstacle::killZ)
        .build();
}

void resetInc()
{
    inc = 0.0F;
    timeBeforeInc = 3.0f;
}

void obstaclePlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Obstacle>();

    cubos.system("move obstacles")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, const Obstacle&, Position&> obstacles) {
            for (auto [ent, obstacle, position] : obstacles)
            {
                glm::vec3 new_vec = obstacle.velocity - glm::vec3(0.0F, 0.0F, inc);
                position.vec += new_vec * dt.value();
                position.vec.y = glm::abs(glm::sin(position.vec.z * 0.15F)) * 1.5F;

                if (position.vec.z < obstacle.killZ)
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

