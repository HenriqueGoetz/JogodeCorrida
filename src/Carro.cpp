#include "Carro.h"
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <iostream>

using namespace std;

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

glm::mat4 matrix_rotate_y(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return glm::mat4(
        // PREENCHA AQUI A MATRIZ DE ROTA��O (3D) EM TORNO DO EIXO Y EM COORD.
        // HOMOG�NEAS, UTILIZANDO OS PAR�METROS c e s
        cos(angle) , 0.0f , -sin(angle) , 0.0f ,  // LINHA 1
        0.0f , 1.0f , 0.0f , 0.0f ,  // LINHA 2
        sin(angle) , 0.0f , cos(angle) , 0.0f ,  // LINHA 3
        0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
    );
}

void Carro::moveCarro(double time)
{
    if(last_time != 0)
    {
        glm::mat4 translation = glm::mat4(
                                    1.0f, 0.0f, 0.0f, 0,      // LINHA 1
                                    0.0f, 1.0f, 0.0f, 0,      // LINHA 2
                                    0.0f, 0.0f, 1.0f, 0,      // LINHA 3
                                    ahead[0], ahead[1], ahead[2], 1.0f       // LINHA 4
                                );
        matrix = (translation) * matrix;
        position = position + ahead;
        position[3] = 1;
    }
    last_time = time;
}

void Carro::turnRight(){
    glm::mat4 translation = glm::mat4(
                                    1.0f, 0.0f, 0.0f, 0,      // LINHA 1
                                    0.0f, 1.0f, 0.0f, 0,      // LINHA 2
                                    0.0f, 0.0f, 1.0f, 0,      // LINHA 3
                                    -position[0], -position[1], -position[2], 1.0f       // LINHA 4
                                );
    glm::mat4 translation2 = glm::mat4(
                                    1.0f, 0.0f, 0.0f, 0,      // LINHA 1
                                    0.0f, 1.0f, 0.0f, 0,      // LINHA 2
                                    0.0f, 0.0f, 1.0f, 0,      // LINHA 3
                                    position[0], position[1], position[2], 1.0f       // LINHA 4
                                );
    glm::mat4 rotation = matrix_rotate_y(-0.3);
    matrix = translation2 * rotation * translation* matrix;
    ahead = matrix * ahead;
}

void Carro::turnLeft(){
    cout << "Truning left" << endl;
}

glm::mat4 Carro::getMatrix()
{
    return matrix;
}
