#pragma once

#include <cubos/engine/prelude.hpp>

struct Player
{
    CUBOS_REFLECT;

    float speed{1.0F};     // Speed of the player
    float laneWidth{1.0F}; // Width of the lane
    int lane{0};           // Current lane
    int targetLane{0};     // Target lane
    bool jetpacked{false}; // Is the player jetpacked?
    bool shielded{false};  // Is the player shielded?
};

void playerPlugin(cubos::engine::Cubos& cubos);
