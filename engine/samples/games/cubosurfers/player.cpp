#include "player.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

static float jetpackTime = 0.0F;

CUBOS_REFLECT_IMPL(Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("Player")
        .withField("speed", &Player::speed)
        .withField("laneWidth", &Player::laneWidth)
        .build();
}

void playerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Player>();

    cubos.system("move player").call([](Input& input, const DeltaTime& dt, Query<Player&, Position&, Rotation&> players) {
        for (auto [player, position, rotation] : players)
        {

            if (player.jetpacked)
            {
                jetpackTime += dt.value();
                if (jetpackTime >= 5.0f)
                {
                    player.jetpacked = false;
                    jetpackTime = 0.0f;
                }
            }

            if (input.pressed("left") && player.lane == player.targetLane)
            {
                player.targetLane = glm::clamp(player.lane - 1, -1, 1);
            }


            if (input.pressed("right") && player.lane == player.targetLane)
            {
                player.targetLane = glm::clamp(player.lane + 1, -1, 1);
            }


            if (player.lane != player.targetLane)
            {
                auto sourceX = static_cast<float>(-player.lane) * player.laneWidth;
                auto targetX = static_cast<float>(-player.targetLane) * player.laneWidth;
                float currentT = (position.vec.x - sourceX) / (targetX - sourceX);
                float newT = glm::min(1.0F, currentT + dt.value() * player.speed);
                position.vec.x = glm::mix(sourceX, targetX, newT);

                if (player.jetpacked)
                {
                    position.vec.y = 16.5F + glm::sin(jetpackTime * 2.0f) * 2.0f;

                    float rotationSpeed;

                    if (targetX > sourceX) { rotationSpeed = glm::radians(-25.0f); }
                    else{ rotationSpeed = glm::radians(25.0f); }

                    glm::quat targetRotation  = glm::angleAxis(rotationSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
                    rotation.quat = glm::slerp(rotation.quat, targetRotation, dt.value() * 5.0f);
                }
                else
                {
                    position.vec.y = glm::sin(currentT * glm::pi<float>()) * 2.0F;
                }

                if (newT == 1.0F)
                {
                    player.lane = player.targetLane;
                }
            }
            else
            {
                if (player.jetpacked)
                {
                    position.vec.y = 16.5F + glm::sin(jetpackTime * 2.0f) * 2.0f;
                }
                else
                {
                    position.vec.y = glm::mix(position.vec.y, 0.0f, dt.value() * 2.0f);
                }
                rotation.quat = glm::slerp(rotation.quat, glm::identity<glm::quat>(), dt.value() * 5.0f);
            }
        }
    });
}
