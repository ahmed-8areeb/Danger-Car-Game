#pragma once

#include <application.hpp>

class ClearColorState: public our::State {
    // onInitialize() function is called once before the state starts
    void onInitialize() override {
        //TODO: Read the color from the configuration file and use it to set the clear color for the window
        //HINT: you can the configuration for the whole application using "getApp()->getConfig()"
        //To see how the clear color is written in the json files, see "config/blue-screen.json"
        //To know how read data from a nlohmann::json object, 
        //look at the following documentation: https://json.nlohmann.me/features/element_access/

        // here we read the json file in blueScreen obj
        const auto& blueScreen = getApp()->getConfig();

        // here we call glClearColor to clear the screen with rgb a vslues that in blue-screen.json
        glClearColor(blueScreen["scene"]["clear-color"]["r"], blueScreen["scene"]["clear-color"]["g"], blueScreen["scene"]["clear-color"]["b"], blueScreen["scene"]["clear-color"]["a"]);
    
    }

    // onDraw(deltaTime) function is called every frame 
    void onDraw(double deltaTime) override {
        //At the start of frame we want to clear the screen. Otherwise we would still see the results from the previous frame.
        glClear(GL_COLOR_BUFFER_BIT);
    }
};