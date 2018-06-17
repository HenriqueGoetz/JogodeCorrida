#ifndef CARRO_H
#define CARRO_H
#include <glm/mat4x4.hpp>

class Carro
{
    private:
         glm::mat4 matrix;
         float speed = 0.5f;
         double last_time;

    public:
        Carro();
        virtual ~Carro();
        void moveCarro(double tempoagora);
        glm::mat4 getMatrix();

};

#endif // CARRO_H
