#ifndef CARRO_H
#define CARRO_H


class Carro
{
    public:
         <glm::mat4> matriz = Matrix_Identity();
         float velocidade = 0.5f;
         double ultimotempo = glfwGetTime();

    public:
        Carro();
        virtual ~Carro();

    public:
        moveCarro(double tempoagora){
            matrix = matriz *
            (tempoagora-(*ultimotempo)*Matrix_Translate(1.0f,0.0f,0.0f));
            ultimotempo = glfwGetTime();
        }

};

#endif // CARRO_H
