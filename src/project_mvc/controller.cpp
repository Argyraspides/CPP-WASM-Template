#include "controller.h"
#include "model.h"

// Completely up to you how you want to implement this. It is your controller, after all.
// See https://github.com/Argyraspides/Telos for an example of a controller implementation in the form of
// safely retrieving/manipulating data from a physics engine

Controller::Controller(Model *model)
{
    m_model = model;
}
