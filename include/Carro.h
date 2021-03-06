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
    float speed = 0.1f;
    float comprimento = 23.0f;
    float largura = 12.0f;
    double last_time;
    glm::vec4 ahead = glm::vec4(0.0,0.0,1.0,0.0);
    glm::vec4 position = glm::vec4(0.0,0.0,0.0,1.0);
    bool testeColisao(glm::vec4 position, glm::vec4 sentido);
    bool cruzouLimites(vector <glm::vec4> pontos);
    bool trapaceou(vector <glm::vec4> pontos);
    bool algumAntesDaChegada(vector <glm::vec4> pontos);
    bool algumDepoisDaChegada(vector <glm::vec4> pontos);
    bool algumAntesDaSaida(vector <glm::vec4> pontos);
    bool algumDepoisDaSaida(vector <glm::vec4> pontos);
    bool estaoNaRetaFinal(vector <glm::vec4> pontos);

public:
    bool Naoinicializado = true;
    Carro();
    virtual ~Carro();
    void moveCarro(double tempoagora);
    bool cruzouChegada();
    glm::mat4 getMatrix();
    void turnRight();
    void turnLeft();
    void moveCarBack();
    glm::vec4 getCameraPosition();
    glm::vec4 getCameraView();
};

#endif // CARRO_H
