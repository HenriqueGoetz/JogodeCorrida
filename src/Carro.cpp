#include "Carro.h"
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

Carro::Carro()
{
    matrix = glm::mat4(
                 0.5f, 0, 0, 0, // COLUNA 1
                 0, 0.5f, 0, 0, // COLUNA 2
                 0, 0, 0.5f, 0, // COLUNA 3
                 0, 0, 0, 1  // COLUNA 4
             );
    turnRight();
    turnRight();
    turnRight();
    turnRight();
    turnRight();
    turnRight();
    turnRight();
    turnRight();



    matrix = matrix * glm::mat4(
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    -4, 0, 0, 1
                );

    position = position + glm::vec4(0,0,-2,0);
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
               // PREENCHA AQUI A MATRIZ DE ROTAÇÃO (3D) EM TORNO DO EIXO Y EM COORD.
               // HOMOGÊNEAS, UTILIZANDO OS PARÂMETROS c e s
               cos(angle), 0.0f, -sin(angle), 0.0f,      // LINHA 1
               0.0f, 1.0f, 0.0f, 0.0f,      // LINHA 2
               sin(angle), 0.0f, cos(angle), 0.0f,      // LINHA 3
               0.0f, 0.0f, 0.0f, 1.0f       // LINHA 4
           );
}

bool Carro::cruzouLimites(vector <glm::vec4> pontos)
{

    for(int i = 0; i < pontos.size(); i++)
    {
        //Blocos externos
        if(pontos[i][0]>=9 || pontos[i][0]<=-9 || pontos[i][2]>=14  || pontos[i][2]<=-4)
        {
            return true;
        }
        //Blocos internos
        if(pontos[i][0]<5 && pontos[i][0]>-5 && pontos[i][2]<10 && pontos[i][2]>0)
        {
            return true;
        }
    }

    return false;
}

bool Carro::testeColisao(glm::vec4 position, glm::vec4 sentido)
{

    vector <glm::vec4> pontos;
    glm::vec4 vetor90graus = glm::vec4(sentido[2],sentido[1],sentido[0],0);

    glm::vec4 vetorsuperiordir = ((comprimento/2)*sentido) + ((largura/2)*vetor90graus);
    glm::vec4 vetorsuperioresq = ((comprimento/2)*sentido) + ((largura/2)*-vetor90graus);
    glm::vec4 vetorinferiordir = ((comprimento/2)*-sentido) + ((largura/2)*vetor90graus);
    glm::vec4 vetorinferioresq = ((comprimento/2)*-sentido) + ((largura/2)*-vetor90graus);

    pontos.push_back(position + (vetorsuperiordir*speed));
    pontos.push_back(position + (vetorinferiordir*speed));
    pontos.push_back(position + (vetorsuperioresq*speed));
    pontos.push_back(position + (vetorinferioresq*speed));

    if(cruzouLimites(pontos))
    {
        return true;
    }
    return false;
}

void Carro::moveCarro(double time)
{
    if(last_time != 0 && !Carro::testeColisao(position,ahead))
    {
        glm::mat4 translation = glm::mat4(
                                    1.0f, 0.0f, 0.0f, 0,      // LINHA 1
                                    0.0f, 1.0f, 0.0f, 0,      // LINHA 2
                                    0.0f, 0.0f, 1.0f, 0,      // LINHA 3
                                    ahead[0]*speed, ahead[1]*speed, ahead[2]*speed, 1.0f       // LINHA 4
                                );
        matrix = (translation) * matrix;
        position = position + speed*ahead;
        position[3] = 1;
    }
    last_time = time;
}

void Carro::turnRight()
{
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
    glm::mat4 rotation = matrix_rotate_y(-0.2);
    matrix = translation2 * rotation * translation* matrix;
    ahead = rotation * ahead;
}

void Carro::turnLeft()
{
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
    glm::mat4 rotation = matrix_rotate_y(0.2);
    matrix = translation2 * rotation * translation* matrix;
    ahead = rotation * ahead;
}

void Carro::moveCarBack()
{
    if(!testeColisao(position, -ahead)){
    glm::mat4 translation = glm::mat4(
                                1.0f, 0.0f, 0.0f, 0,      // LINHA 1
                                0.0f, 1.0f, 0.0f, 0,      // LINHA 2
                                0.0f, 0.0f, 1.0f, 0,      // LINHA 3
                                -ahead[0], -ahead[1], -ahead[2], 1.0f       // LINHA 4
                            );
    matrix = (translation) * matrix;
    position = position - ahead;
    position[3] = 1;
    }
}

glm::mat4 Carro::getMatrix()
{
    return matrix;
}

glm::vec4 Carro::getCameraPosition()
{
    glm::vec4 result = position;
    result[1] += 3;
    result = result -8.0f*ahead;
    return result;
}

glm::vec4 Carro::getCameraView()
{
    return ahead;
}
