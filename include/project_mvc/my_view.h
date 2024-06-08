#pragma once
#include <SDL.h>
#include "my_view_interface.h"
#include "controller.h"
#include "imgui.h"

class MyView : public MyViewInterface
{

public:
    MyView(Controller *controller) : m_controller(controller){}; // Constructor

    Controller *m_controller; // Here is your controller to manipulate and/or retrieve data from the model!

    void RenderImGui(ImGuiIO &io) override;                               // Render ImGui stuff here
    void RenderSDL(SDL_Renderer &renderer) override;                      // Render SDL stuff here
    void HandleSDLEvents(SDL_Event &event) override;                      // Handle SDL events here
    void SDLSetup(SDL_Renderer **renderer, SDL_Window **window) override; // Set up the SDL renderer and window stuff here
    ImGuiIO &ImGuiSetup() override;                                       // Set up ImGui stuff here

    void DrawCircle(SDL_Renderer &renderer); // Example function to draw a circle using SDL2
    void HandlePKey(SDL_Event &event);       // Example function to handle the 'p' key being pressed
    void POSTRequest();                      // Example function to make a POST request using Hermes
};
