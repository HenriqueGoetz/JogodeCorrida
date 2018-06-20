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

    Naoinicializado = false;

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
    bool retorno = false;

    for(int i = 0; i < pontos.size(); i++)
    {
        //printf("\t\nPonto %d: %f ,%f",i, pontos[i][0], pontos[i][2]);
        //Blocos externos
        if(pontos[i][0]>=9 || pontos[i][0]<=-9 || pontos[i][2]>=14  || pontos[i][2]<=-4)
        {
            printf("\n\tUltrapassaria bloco externo.");
            retorno =  true;
        }
        //Blocos internos
        if(pontos[i][0]<=5 && pontos[i][0]>=-5 && pontos[i][2]<=10 && pontos[i][2]>=0)
        {
            printf("\n\tUltrapassaria bloco interno.");
            retorno = true;
        }
    }

    return retorno;
}

bool Carro::testeColisao(glm::vec4 position, glm::vec4 sentido)
{

    vector <glm::vec4> pontos;
    //printf("\n\tAhead:        %f, %f, %f",sentido[0],sentido[1],sentido[2]);
    glm::vec4 vetor90graus = glm::vec4(sentido[2],sentido[1],(-1*sentido[0]),0);
    //printf("\n\tvetor90graus: %f, %f, %f",vetor90graus[0],vetor90graus[1],vetor90graus[2]);
    //printf("\n\tLargura/2:     %f", largura/2);
    //printf("\n\tComprimento/2: %f", comprimento/2);
    /*
    glm::vec4 ponto01 = position + (((comprimento/2)*sentido)+((largura/2)*vetor90graus));
    glm::vec4 ponto02 = position + (((comprimento/2)*sentido)-((largura/2)*vetor90graus));
    glm::vec4 ponto03 = position + (-((comprimento/2)*sentido)+((largura/2)*vetor90graus));
    glm::vec4 ponto04 = position + (-((comprimento/2)*sentido)-((largura/2)*vetor90graus));
    printf("\n\tPonto01: %f, %f, %f", ponto01[0], ponto01[1], ponto01[2]);
    printf("\n\tPonto02: %f, %f, %f", ponto02[0], ponto02[1], ponto02[2]);
    printf("\n\tPonto03: %f, %f, %f", ponto03[0], ponto03[1], ponto03[2]);
    printf("\n\tPonto04: %f, %f, %f", ponto04[0], ponto04[1], ponto04[2]);
    */

    glm::vec4 vetorsuperiordir = ((comprimento/2)*sentido)+((largura/2)*vetor90graus);
    glm::vec4 vetorsuperioresq = ((comprimento/2)*sentido)-((largura/2)*vetor90graus);
    glm::vec4 vetorinferiordir = -((comprimento/2)*sentido)+((largura/2)*vetor90graus);
    glm::vec4 vetorinferioresq = -((comprimento/2)*sentido)-((largura/2)*vetor90graus);

    pontos.push_back(position + (vetorsuperiordir*speed));
    pontos.push_back(position + (vetorsuperioresq*speed));
    pontos.push_back(position + (vetorinferiordir*speed));
    pontos.push_back(position + (vetorinferioresq*speed));

    glm::vec4 ponto1 = position + vetorsuperiordir*speed;
    glm::vec4 ponto2 = position + vetorsuperioresq*speed;
    glm::vec4 ponto3 = position + vetorinferiordir*speed;
    glm::vec4 ponto4 = position + vetorinferioresq*speed;

    printf("\n\tPosicao superior direita:  %f , %f", ponto1[0], ponto1[2]);
    printf("\n\tPosicao superior esquerda: %f , %f", ponto2[0], ponto2[2]);
    printf("\n\tPosicao inferior direita:  %f , %f", ponto3[0], ponto3[2]);
    printf("\n\tPosicao inferior esquerda: %f , %f", ponto4[0], ponto4[2]);

    if(cruzouLimites(pontos))
    {
        //printf("Colidiu.\n");
        return true;
    }
    return false;
}

void Carro::moveCarro(double time)
{
    printf("\n\t Posicao Atual: %f , %f",position[0], position[2]);
    if(!testeColisao(position+(speed*ahead),ahead))
    {
        //printf("Moveu\n");
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
     printf("\n\t Posicao Atual: %f , %f",position[0], position[2]);
    glm::mat4 rotation = matrix_rotate_y(-0.2);
    //if(!testeColisao(position,ahead*rotation) || Naoinicializado)
    //{
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

    matrix = translation2 * rotation * translation* matrix;
    ahead = rotation * ahead;
    //}
}

void Carro::turnLeft()
{
     printf("\n\t Posicao Atual: %f , %f",position[0], position[2]);
    glm::mat4 rotation = matrix_rotate_y(0.2);
    if(!testeColisao(position, ahead*rotation))
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

    matrix = translation2 * rotation * translation* matrix;
    ahead = rotation * ahead;
    }
}
void Carro::moveCarBack()
{
    printf("\n\t Posicao Atual: %f , %f",position[0], position[2]);
    if(!testeColisao(position-(ahead*speed), -ahead))
    {
        glm::mat4 translation = glm::mat4(
                                    1.0f, 0.0f, 0.0f, 0,      // LINHA 1
                                    0.0f, 1.0f, 0.0f, 0,      // LINHA 2
                                    0.0f, 0.0f, 1.0f, 0,      // LINHA 3
                                    -ahead[0]*speed, -ahead[1]*speed, -ahead[2]*speed, 1.0f       // LINHA 4
                                );
        matrix = (translation) * matrix;
        position = position - (ahead*speed);
        position[3] = 1;
    }
}

bool Carro::cruzouChegada()
{
    vector <glm::vec4> pontos;
    glm::vec4 vetor90graus = glm::vec4(ahead[2],ahead[1],(-1*ahead[0]),0);

    glm::vec4 vetorsuperiordir = ((comprimento/2)*ahead)+((largura/2)*vetor90graus);
    glm::vec4 vetorsuperioresq = ((comprimento/2)*ahead)-((largura/2)*vetor90graus);
    glm::vec4 vetorinferiordir = -((comprimento/2)*ahead)+((largura/2)*vetor90graus);
    glm::vec4 vetorinferioresq = -((comprimento/2)*ahead)-((largura/2)*vetor90graus);

    glm::vec4 ponto1 = position +vetorsuperiordir*speed;

    //printf("\n\tPonto1 = %f, %f, %f", ponto1[0],ponto1[1],ponto1[2]);

    pontos.push_back(position + vetorsuperiordir*speed);
    pontos.push_back(position + vetorsuperioresq*speed);
    pontos.push_back(position + vetorinferiordir*speed);
    pontos.push_back(position + vetorinferioresq*speed);


    if(!estaoNaRetaFinal(pontos))
    {
        return false;
    }

    printf("Aq\n");
    if(algumAntesDaChegada(pontos) && algumDepoisDaChegada(pontos))
    {
        return true;
    }

    return false;
}
bool Carro::estaoNaRetaFinal(vector <glm::vec4> pontos)
{

    for(int i=0; i < pontos.size(); i++)
    {
        if(pontos[i][2]>0)
        {
            return false;
        }
    }
    return true;
}

bool Carro::algumAntesDaChegada(vector <glm::vec4> pontos)
{

    for(int i=0; i < pontos.size(); i++)
    {
        if(pontos[i][0]>=3)
        {
            return true;
        }
    }

    return false;
}

bool Carro::algumDepoisDaChegada(vector <glm::vec4> pontos)
{

    for(int i=0; i < pontos.size(); i++)
    {
        //printf("\n\t%d - %f", i, pontos[i][0]);
        if(pontos[i][0]<3)
        {
            return true;
        }
    }

    return false;
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
