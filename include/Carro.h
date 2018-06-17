#ifndef CARRO_H
#define CARRO_H


class Carro
{
    public:
         <glm::mat4> matriz = Matrix_Identity();
         float velocidade = 0.5f;

    public:
        Carro();
        virtual ~Carro();

    public:
        moveCarro(double tempoagora, double *ultimo){
            matrix = matriz *
            (tempoagora-(*ultimotempo)*Matrix_Translate(1.0f,0.0f,0.0f));
        }

};

#endif // CARRO_H
