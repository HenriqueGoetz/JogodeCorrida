#include "Carro.h"
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>

Carro::Carro()
{
    matrix = glm::mat4(
        1, 0, 0, 0, // COLUNA 1
        0, 1, 0, 0, // COLUNA 2
        0, 0, 1, 0, // COLUNA 3
        0, 0, 0, 1  // COLUNA 4
    );;
    last_time = glfwGetTime();
}

Carro::~Carro()
{
    //dtor
}

void Carro::moveCarro(double time){
    glm::mat4 translation = glm::mat4(
        1.0f , 0.0f , 0.0f , 1 ,  // LINHA 1
        0.0f , 1.0f , 0.0f , 0 ,  // LINHA 2
        0.0f , 0.0f , 1.0f , 0 ,  // LINHA 3
        0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
    );
    matrix = matrix * ((float)(time - last_time)* translation);
    last_time = time;
}

glm::mat4 Carro::getMatrix(){
    return matrix;
}
