#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>


#include "obstacle.hpp"
#include "player.hpp"
#include "spawner.hpp"
#include "powerup.hpp"

using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("ee5bb451-05b7-430f-a641-a746f7009eef");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("101da567-3d23-46ae-a391-c10ec00e8718");
static const Asset<InputBindings> InputBindingsAsset = AnyAsset("b20900a4-20ee-4caa-8830-14585050bead");

static const Asset<VoxelGrid> shieldAsset = AnyAsset("4892c2f3-10b3-4ca7-9de3-822b77a0ba7e");
static const Asset<VoxelGrid> jetpackAsset = AnyAsset("c7263b46-be18-47c2-b3ef-05592b2e9dec");

static bool shielded = false;

int main()
{
    Cubos cubos{};

    cubos.plugin(defaultsPlugin);
    cubos.plugin(spawnerPlugin);
    cubos.plugin(obstaclePlugin);
    cubos.plugin(playerPlugin);
    cubos.plugin(powerUpPlugin);
    
    cubos.startupSystem("configure settings").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("set the palette, environment, input bindings and spawn the scene")
        .tagged(assetsTag)
        .call([](Commands commands, const Assets& assets, RenderPalette& palette, Input& input,
                 RenderEnvironment& environment) {
            palette.asset = PaletteAsset;
            environment.ambient = {0.1F, 0.1F, 0.1F};
            environment.skyGradient[0] = {0.2F, 0.4F, 0.8F};
            environment.skyGradient[1] = {0.6F, 0.6F, 0.8F};
            input.bind(*assets.read(InputBindingsAsset));
            commands.spawn(assets.read(SceneAsset)->blueprint);
        });

    cubos.system("restart the game on input")
        .call([](Commands cmds, const Assets& assets, const Input& input, Query<Entity> all) {
            if (input.justPressed("restart"))
            {
                for (auto [ent] : all)
                {
                    cmds.destroy(ent);
                }

                cmds.spawn(assets.read(SceneAsset)->blueprint);
                resetInc();
            }
        });

    cubos.system("detect player vs obstacle collisions")
        .call([](Query<const Player&, const CollidingWith&, const Obstacle&> collisions, Commands cmds, const Assets& assets, Query<Entity> all, Query<Entity, const Obstacle&> obstacles) {
            for (auto [player, collidingWith, obstacle] : collisions)
            {
                if (!shielded)
                {
                    CUBOS_INFO("Player collided with an obstacle!");
                    (void)player; // here to shut up 'unused variable warning', you can remove it
                    
                    for (auto [ent] : all)
                    {
                        cmds.destroy(ent);
                    }

                    cmds.spawn(assets.read(SceneAsset)->blueprint);
                    resetInc();
                } 
                else
                {
                    for(auto [ent, obstacle2] : obstacles)
                    {
                        if (&obstacle == &obstacle2)
                        {
                            cmds.destroy(ent);
                        }
                    }
                }

                shielded = false;
            }
        });

    cubos.system("detect player vs obstacle collisions")
        .call([](Query<const Player&, const CollidingWith&, const PowerUp&> collisions, Query<Entity, const PowerUp&> powerUps, Commands cmds, Query<Entity, const Player&, RenderVoxelGrid&> voxels) {
            for (auto [player, collidingWith, powerUp] : collisions)
            {
                CUBOS_INFO("Player pick a powerup!");
                (void)player; // here to shut up 'unused variable warning', you can remove it

                for (auto [ent, powerUp2] : powerUps)
                {
                    if (&powerUp2 == &powerUp)
                    {
                        shielded = true;
                        cmds.destroy(ent);

                        
                        for (auto [ent, player, voxel] : voxels)
                        {  
                            voxel.asset = shieldAsset;
                            cmds.add(ent, LoadRenderVoxels{});
                        }
                    }
                }
                
            }
        });

    cubos.run();
}
