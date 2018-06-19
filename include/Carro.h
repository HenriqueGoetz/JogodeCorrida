#ifndef CARRO_H
#define CARRO_H
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <vector>


using namespace std;

class Carro
{
    private:
         glm::mat4 matrix;
         float speed = 0.5f;
         float comprimento = 6;
         float largura = 2.5;
         double last_time;
         glm::vec4 ahead = glm::vec4(0.0,0.0,1.0,0.0);
         glm::vec4 position = glm::vec4(0.0,0.0,0.0,1.0);
         bool testeColisao(glm::vec4 position, glm::vec4 sentido);
         bool cruzouLimites(vector <glm::vec4> pontos);

    public:
        Carro();
        virtual ~Carro();
        void moveCarro(double tempoagora);
        glm::mat4 getMatrix();
        void turnRight();
        void turnLeft();
        void moveCarBack();
        glm::vec4 getCameraPosition();
        glm::vec4 getCameraView();
};

#endif // CARRO_H
