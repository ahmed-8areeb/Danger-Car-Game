#pragma once
#include <iostream>
#include "../ecs/world.hpp"
#include "../components/car.hpp"
#include "../components/player.hpp"

#include "../components/collision.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

   // this class is responsible for game controller 
    class GameControllerSystem
    {
        Application *app; // The application in which the state runs
        // bool mouse_locked = false; // Is the mouse locked
        int time;
        bool isDecreasing = false;
    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;
        }

        bool checkWin(World *world){
            PlayerComponent *player = world->getPlayer();
            // If there is no entity of a car we can do nothing so we return
            if (!(player))
                return false;
            //TODO:: make Total coins
            if(player->coins >= player->totalCoins/2){
                player->state = 2;
                return true;
            }
            player->state = 1;
            return false;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        bool checkHealth(World *world,float deltaTime)
        {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break

            PlayerComponent *player = world->getPlayer();
            // If there is no entity of a car we can do nothing so we return
            if (!(player))
                return false;
            Entity *playerEntity = player->getOwner();
            glm::vec3 &healthScale = playerEntity->localTransform.scale;
            if(player->health<=0){
                player->state = 0;
                return false;
            }

            // decreae the health of the player as time passes 
            if(isDecreasing){
                time-=deltaTime;
                if(time <= 0)
                    isDecreasing=false;
            }else{
                player->health -= 0.1;
                healthScale.x -= 0.1/100;
                time = 2;           // decrease the health of the player every two seconds
                isDecreasing = true;
            }
            return true;
        }

        int getPlayerStatus(World *world){
            PlayerComponent *player = world->getPlayer();
            return player->state;
        }
    };
}
