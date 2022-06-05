#pragma once
#include<iostream>
#include "../ecs/world.hpp"
#include "../components/car.hpp"

#include "../application.hpp"

#include "../components/collision.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>


namespace our
{

    // this file is responsible for car controller 
    class CarControllerSystem {
        Application* app; // The application in which the state runs

    public:
        bool jumped = false;
        double seconds=1;
        double jumpTime = seconds/2.0;
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent 
        void update(World* world, float deltaTime,bool &finished) {
           
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            CarComponent* car = world->getCar();
            // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
            if(!(car)) return;
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            Entity* entity = car->getOwner();

     
            // We get a reference to the entity's position and rotation
            glm::vec3& position = entity->localTransform.position;
            glm::vec3& rotation = entity->localTransform.rotation;
            glm::vec3& scale = entity->localTransform.scale;
            // glm::vec3 velocity =entity->localTransform.velocity;

          

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = entity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)), 
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            // std::cout<<"pos: "<<position.z<<"\n";

            // A & D moves the player left or right 
            if(app->getKeyboard().isPressed(GLFW_KEY_D) && position.x>-10.0f) position += right * (deltaTime *4);
            if(app->getKeyboard().isPressed(GLFW_KEY_A)  && position.x<10.0f) position -= right * (deltaTime * 4);
            if(app->getKeyboard().isPressed(GLFW_KEY_X)) rotation.y += glm::radians(1.0f);

            // jumping 
            if(jumped){
                seconds-=deltaTime;
                if(seconds<=0) jumped=false;
                if(jumpTime <= seconds)
                    position.y += 0.1;
                else position.y -= 0.1;
                position.z += 0.05;  
            }else if(app->getKeyboard().isPressed(GLFW_KEY_W)){
                position.y = 1.5;
                jumped = true;
                seconds = 1;
            }
            // check if car reaches the finsh line
            finished = checkEnd(world,position.z);
        }
        
        bool checkEnd(World* world,float posZ){
             for (auto entity : world->getEntities())
                if (auto oEntity = entity->getComponent<CollisionComponent>(); oEntity)
                    if(oEntity->obstucaseType=="finish"){
                        Entity *obsEntity = oEntity->getOwner();
                        if(posZ >= obsEntity->localTransform.position.z)
                        {
                            return true;
                        }
                    }
            return false;
        }
    };

}
