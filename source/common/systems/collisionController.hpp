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

    // this file responsible for collision detection and handling between cars and other objects 
    class CollisionControllerSystem
    {
        Application *app; // The application in which the state runs
        

    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;
        }

        // this function handle the collision of the car with the obstacles
        bool checkCollision(World *world, bool &finished, std::vector<bool> &collisionMarker, std::vector<Entity *> &coinBags)
        {
            
            bool Died = false;
            PlayerComponent *player = nullptr;
            for (auto entity : world->getEntities())
            {
                player = entity->getComponent<PlayerComponent>();
                if (player)
                    break;
            }
            // If there is no entity of a car we can do nothing so we return
            if (!(player))
                return true;
            //get player entity
            Entity *playerEntity = player->getOwner();
            // get player health scale
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
                return true;
            Entity *carEntity = car->getOwner();

            // We get a reference to the entity's position and rotation
            glm::vec3 &carPosition = carEntity->localTransform.position;
            glm::vec3 &carRotation = carEntity->localTransform.rotation;
            glm::vec3 &carScale = carEntity->localTransform.scale;
            // get the minium and maxium dimesions of car
            float minCarX = carPosition.x + carEntity->getComponent<MeshRendererComponent>()->mesh->minX;
            float maxCarX = carPosition.x + carEntity->getComponent<MeshRendererComponent>()->mesh->maxX;

            int i = 0;
            for (auto entity : world->getEntities())
            {
                if (auto oEntity = entity->getComponent<CollisionComponent>(); oEntity)
                {
                    // if this component is marked as collision so will not check for each collision again
                    if (collisionMarker[i])
                    {
                        i++;
                        continue;
                    }
                    Entity *objEntity = oEntity->getOwner();

                    glm::vec3 &objPosition = objEntity->localTransform.position;
                    glm::vec3 &objScale = objEntity->localTransform.scale;
                    float minObsX = objPosition.x + objEntity->getComponent<MeshRendererComponent>()->mesh->minX;
                    float maxObsX = objPosition.x + objEntity->getComponent<MeshRendererComponent>()->mesh->maxX;

                    //  AABB Collision Detection
                    // bool collisionX = carPosition.x + (maxCarX - minCarX)*carScale.x/2 >= objPosition.x &&
                    //                   objPosition.x + (maxObsX - minObsX)*objScale.x/2 >= carPosition.x;

                    // bool collisionX = (minCarX>=minObsX &&minCarX<=maxObsX) || (maxCarX>=minObsX &&maxCarX<=maxObsX);

                    bool collisionX = carPosition.x + 5.0 >= objPosition.x &&
                                      objPosition.x + 5.0 >= carPosition.x;

                    // collision y-axis?
                    bool collisionY = carPosition.y - 1.5 < objPosition.y + objScale.y / 2;

                    // std::cout<<carPosition.y<<" "<<objPosition.y+objScale.y/2<<"\n";
                    bool collisionZ = carPosition.z + 2 * carScale.z >= objPosition.z &&
                                      objPosition.z + 3.0 >= carPosition.z;

                    // collision only if on both axes
                    if (collisionX && collisionZ)
                    {
                        // damage the player health if the collision is danger or tree in the game
                        if (oEntity->obstucaseType == "danger" || (oEntity->obstucaseType == "tree" && collisionY))
                        {

                            // get damage of the obstacle
                            int damage = oEntity->effect;
                            // decrease the health of the player
                            player->health -= damage;
                            // check if the player is dead
                            if (player->health <= 0)
                            {
                                player->health = 0;
                                healthScale.x = 0;
                                // TODO: game over
                                player->state = 0;
                                Died = true;
                            }
                            else
                            {
                                healthScale.x -= float(damage) / 100.0;
                            }

                            // objPosition.z -= 20;
                            // objPosition.y -= 20;
                            // mark the component as collision done
                            collisionMarker[i] = true;
                            // avoid to draw  the obstacle
                            objEntity->canDraw = false;
                        }
                        else if (oEntity->obstucaseType == "heart")
                        {
                            // collision logic for heart
                            int bouns = oEntity->effect;
                            player->health += bouns;
                            if (player->health > 100) // if the health is greater than 100, set it to 100
                            {
                                player->health = 100;
                                healthScale.x = 1;
                            }
                            else
                            {
                                healthScale.x += float(bouns) / 100.0;
                            }
                            // objPosition.z -= 20;
                            // objPosition.y -= 20;
                            collisionMarker[i] = true;
                            objEntity->canDraw = false;
                        }
                        else if (oEntity->obstucaseType == "coins") 
                        {
                            player->coins++;
                            // objPosition.z -= 20;
                            // objPosition.y -= 20;
                            collisionMarker[i] = true;
                            if (coinBags.size() > 0)  // if there is coins remains
                            {
                                Entity *bag = coinBags.back(); // get the last coin bag
                                bag->canDraw = false;   
                                coinBags.pop_back();
                            }
                            objEntity->canDraw = false;
                        }
                        else if (oEntity->obstucaseType == "finish")
                        {
                            finished = true; // set the game as finished
                        }

                        break;
                    }
                    i++;
                }
            }
            return Died;
        }
    };
}
