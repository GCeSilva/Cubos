#pragma once

#include <glm/vec3.hpp>

#include <cubos/engine/prelude.hpp>

struct PowerUp
{
    CUBOS_REFLECT;

    glm::vec3 velocity{0.0F, 0.0F, -1.0F};
    float killZ{0.0F};
};

void powerUpPlugin(cubos::engine::Cubos& cubos);

