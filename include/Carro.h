#ifndef CARRO_H
#define CARRO_H


class Carro
{
<<<<<<< HEAD
    private:
         glm::mat4 matrix = Matrix_Identity();
         float speed = 0.5f;
         double last_time;
=======
    public:
         <glm::mat4> matriz = Matrix_Identity();
         float velocidade = 0.5f;
         double ultimotempo = glfwGetTime();
>>>>>>> 0ecabcf0fcf0861acd4d7e9182d6aac603c45dc8

    public:
        Carro();
        virtual ~Carro();
<<<<<<< HEAD
        moveCarro(double tempoagora);
=======

    public:
        moveCarro(double tempoagora){
            matrix = matriz *
            (tempoagora-(*ultimotempo)*Matrix_Translate(1.0f,0.0f,0.0f));
            ultimotempo = glfwGetTime();
        }
>>>>>>> 0ecabcf0fcf0861acd4d7e9182d6aac603c45dc8

};

#endif // CARRO_H
