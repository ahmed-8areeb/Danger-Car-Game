#pragma once

#include <application.hpp>
#include <ecs/world.hpp>
#include "play-state.hpp"
#include "../common/components/player.hpp"
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/gameController.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
#include "imgui.h"
#include <string>
#include <cstring>



// This state shows how to use the ECS framework and deserialization.
class GameOverState : public our::State
{

  our::World world;
  our::ForwardRenderer renderer;
  our::FreeCameraControllerSystem cameraController;
  our::MovementSystem movementSystem;
  our::PlayerComponent * player;
  our::GameControllerSystem * game;


  
  void onImmediateGui() override
  {
    int state = getApp()->state;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui::Begin("gameover State");

    ImGui::SetWindowFontScale(4.0f);
    ImGui::SetWindowSize(ImVec2((float)900, (float)500));

    if(state == 0){
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
      ImGui::Text("Game Over");
      ImGui::Text("your car is totally damaged");
      ImGui::PopStyleColor();
    }
    else if(state == 1){
       ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
      ImGui::Text("Game Over");
       ImGui::Text("you failed to collect at least half the coins");
       ImGui::PopStyleColor();
    }else if(state == 2){
       ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
       ImGui::Text("congratulations");
       ImGui::Text("you collect all the coins");
       ImGui::PopStyleColor();
    }
 
    
    ImGui::Text(" ");
    ImGui::Text("click yes or press enter to exit the game");
    ImGui::Text(" ");
    if (ImGui::Button("Yes")||getApp()->getKeyboard().isPressed(GLFW_KEY_ENTER))
    {
      exit(1);
    }
    ImGui::End();
  }
  
  void onInitialize() override
  {
    // First of all, we get the scene configuration from the app config
    auto &config = getApp()->getConfig()["scene"];
    // If we have assets in the scene config, we deserialize them
    if (config.contains("assets"))
    {
      our::deserializeAllAssets(config["assets"]);
    }
    // If we have a world in the scene config, we use it to populate our world
    if (config.contains("menu"))
    {
      world.deserialize(config["menu"]);
    }
  }

  void onDraw(double deltaTime) override
  {
    // And finally we use the renderer system to draw the scene
    renderer.render(&world);
  }

  void onDestroy() override
  {
    // Don't forget to destroy the renderer
    renderer.destroy();
    // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
    // and we delete all the loaded assets to free memory on the RAM and the VRAM
    our::clearAllAssets();
  }

};