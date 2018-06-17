#ifndef CARRO_H
#define CARRO_H


class Carro
{
    private:
         glm::mat4 matrix = Matrix_Identity();
         float speed = 0.5f;
         double last_time = glfwGetTime();;

    public:
        Carro();
        virtual ~Carro();
        moveCarro(double tempoagora);

};

#endif // CARRO_H
