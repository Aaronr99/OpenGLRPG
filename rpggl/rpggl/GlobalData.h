#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"
#include "InputManager.h"

namespace GlobalData
{
    extern const unsigned int SCR_WIDTH;
    extern const unsigned int SCR_HEIGHT;
    extern Camera camera;
    extern InputManager inputManager;
}



