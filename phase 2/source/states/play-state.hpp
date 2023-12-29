#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/camera-lock.hpp>
#include <systems/player-movement.hpp>
#include <systems/car-movement.hpp>
#include <systems/win.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::MovementSystem movement;
    our::CameraLockSystem cameraLock;
    our::PlayerMovementSystem playerMovementSystem;
    our::CarMovementSystem carMovementSystem;
    our::WinSystem winSystem;

    void onInitialize(std::string msg) override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraLock.enter(getApp());
        playerMovementSystem.enter(getApp());
        carMovementSystem.enter(getApp());
        winSystem.enter(getApp());

        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        if (msg == "PostProcessingRequested"){
            renderer.requestPostProcessing();
        }
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        //movementSystem.update(&world, (float)deltaTime);
        playerMovementSystem.update(&world, (float)deltaTime);
        if (!playerMovementSystem.win)
            carMovementSystem.update(&world, (float)deltaTime);
        cameraLock.update(&world, (float)deltaTime);
        movement.update(&world, (float)deltaTime);

        if (playerMovementSystem.win)
            winSystem.update(&world, (float)deltaTime);
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraLock.exit();
        playerMovementSystem.exit();
        carMovementSystem.exit();
        winSystem.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};