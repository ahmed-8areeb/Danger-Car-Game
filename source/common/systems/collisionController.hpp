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

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic.
    // For more information, see "common/components/free-camera-controller.hpp"
    class CollisionControllerSystem
    {
        Application *app; // The application in which the state runs
        // bool mouse_locked = false; // Is the mouse locked

    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;
        }

        // bool CheckCollision(Entity *entity1, GameObject &two) // AABB - AABB collision
        // {
        //     // collision x-axis?
        //     bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        //                       two.Position.x + two.Size.x >= one.Position.x;
        //     // collision y-axis?
        //     bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        //                       two.Position.y + two.Size.y >= one.Position.y;
        //     // collision only if on both axes
        //     return collisionX && collisionY;
        // }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        void checkCollision(World *world,bool &collision)
        {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            
            PlayerComponent *player = nullptr;
            for (auto entity : world->getEntities())
            {
                player = entity->getComponent<PlayerComponent>();
                if (player)
                    break;
            }
            // If there is no entity of a car we can do nothing so we return
            if (!(player))
                return;
            Entity *playerEntity = player->getOwner();
            glm::vec3 &healthScale = playerEntity->localTransform.scale;

            std::vector<Entity *> obstacles;
            // get car component
            CarComponent *car = nullptr;
            for (auto entity : world->getEntities())
            {
                car = entity->getComponent<CarComponent>();
                if (car)
                    break;
            }
            // If there is no entity of a car we can do nothing so we return
            if (!(car))
                return;

            Entity *carEntity = car->getOwner();

            // We get a reference to the entity's position and rotation
            glm::vec3 &carPosition = carEntity->localTransform.position;
            glm::vec3 &carRotation = carEntity->localTransform.rotation;
            glm::vec3 &carScale = carEntity->localTransform.scale;
            

            for (auto entity : world->getEntities())
            {
                if (auto oEntity = entity->getComponent<CollisionComponent>(); oEntity)
                {
                    Entity *objEntity = oEntity->getOwner();
                    glm::vec3 &objPosition = objEntity->localTransform.position;
                    glm::vec3 &objScale = objEntity->localTransform.scale;

                    bool collisionX = carPosition.x + 5.0 >= objPosition.x &&
                                      objPosition.x + 5.0 >= carPosition.x;
                    // collision y-axis?
                    bool collisionY = carPosition.y - 1.5  < objPosition.y+objScale.y/2;
                                     
                    // std::cout<<carPosition.y<<" "<<objPosition.y+objScale.y/2<<"\n";
                    bool collisionZ = carPosition.z + 3.0 >= objPosition.z &&
                                      objPosition.z + 3.0 >= carPosition.z;

                    // collision only if on both axes
                    if (collisionX && collisionZ)
                    {
                        // collision logic
                        if (oEntity->obstucaseType == "danger"||(oEntity->obstucaseType == "tree"&&collisionY))
                        {

                            // collision logic for danger
                            int damage = oEntity->effect;
                            player->health -= damage;
                            if (player->health <= 0)
                            {
                              player->health = 0;
                              healthScale.x = 0;
                              //TODO: game over
                              collision = true;
                              
                            }
                            else{
                                healthScale.x -= float(damage)/100.0;
                            }   
                            //TODO: return the z to +10
                            objPosition.z -= 20;
                            objPosition.y -= 20;
                        }
                        else if (oEntity->obstucaseType == "heart")
                        {
                            // collision logic for heart
                            int bouns = oEntity->effect;
                            player->health += bouns;
                            if (player->health > 100)
                            {
                                player->health = 100;
                                healthScale.x = 1;
                            }
                            else {
                                healthScale.x += float(bouns)/100.0;;
                            }
                            objPosition.z -= 20;
                            objPosition.y -= 20;
                        }
                        else if(oEntity->obstucaseType == "finish"){
                            //TODO: game over
                              collision = true;
                        }
                        break;
                    }
                }
            }
        }
    };
}
