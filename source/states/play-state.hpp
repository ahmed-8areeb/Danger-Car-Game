#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/carController.hpp>
#include <systems/collisionController.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CarControllerSystem carController;
    our::CollisionControllerSystem collisionController;
    bool alreadyInitialized = false;
    void onInitialize() override {
        //if(alreadyInitialized) return ;
        //alreadyInitialized = true;
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
        
        cameraController.enter(getApp());
        carController.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
       cameraController.update(&world, (float)deltaTime);

        carController.update(&world, (float)deltaTime);
        bool Collision = false;
        collisionController.checkCollision(&world,Collision);
        if(Collision)
            getApp()->changeState("gameOver");
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

         
        if (getApp()->getKeyboard().isPressed(GLFW_KEY_ESCAPE))
        {
            getApp()->changeState("menu");
        }
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        // renderer.destroy();
        // // // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        // cameraController.exit();
        // // // and we delete all the loaded assets to free memory on the RAM and the VRAM
        // our::clearAllAssets();
    }
};