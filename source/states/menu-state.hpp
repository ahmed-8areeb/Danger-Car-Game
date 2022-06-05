#pragma once

#include <application.hpp>
#include <ecs/world.hpp>
#include "play-state.hpp"
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
#include "imgui.h"


class MenuState : public our::State
{

  our::World world;
  our::ForwardRenderer renderer;
  our::FreeCameraControllerSystem cameraController;
  our::MovementSystem movementSystem;
  
  /**
   * @brief create the menu state window and draw the menu
   * 
   */
  void onImmediateGui() override
  {
    
    int score = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui::Begin("Menu State");
    ImGui::SetWindowFontScale(3.0f);
    ImGui::SetWindowSize(ImVec2((float)900, (float)500));
    // ImGui::Text("Welcome to our game");
    // ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    ImGui::Text(" ");
    ImGui::Text("Welcome to our game");
    ImGui::PopStyleColor();
    ImGui::Text("you should collect at least half\n the avaliable coins");
    if (ImGui::Button("Start") )
    {
      this->getApp()->changeState("main");
    }
     ImGui::Text(" ");
    ImGui::Text("Do you want to exit the game ?");
     ImGui::Text(" ");
    if (ImGui::Button("Yes"))
    {
      exit(1);
    }
    ImGui::End();
  }

  // void onInitialize() override
  // {
  //   // First of all, we get the scene configuration from the app config
  //   auto &config = getApp()->getConfig()["scene"];
  //   // If we have assets in the scene config, we deserialize them
  //   if (config.contains("assets"))
  //   {
  //     our::deserializeAllAssets(config["assets"]);
  //   }
  //   // If we have a world in the scene config, we use it to populate our world
  //   if (config.contains("menu"))
  //   {
  //     world.deserialize(config["menu"]);
  //   }
  // }

  // void onDraw(double deltaTime) override
  // {
  //   // And finally we use the renderer system to draw the scene
  //   renderer.render(&world);
  // }

  // void onDestroy() override
  // {
  //   // Don't forget to destroy the renderer
  //   renderer.destroy();
  //   // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
  //   // and we delete all the loaded assets to free memory on the RAM and the VRAM
  //   our::clearAllAssets();
  // }
};