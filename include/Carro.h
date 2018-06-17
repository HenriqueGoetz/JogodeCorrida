#ifndef CARRO_H
#define CARRO_H
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

class Carro
{
    private:
         glm::mat4 matrix;
         float speed = 0.5f;
         double last_time;
         glm::vec4 ahead = glm::vec4(0.0,0.0,1.0,1.0);

    public:
        Carro();
        virtual ~Carro();
        void moveCarro(double tempoagora);
        glm::mat4 getMatrix();
        void turnRight();
        void turnLeft();

};

#endif // CARRO_H
