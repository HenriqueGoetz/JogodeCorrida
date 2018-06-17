#include "Carro.h"

Carro::Carro()
{
    //ctor
}

Carro::~Carro()
{
    //dtor
}


moveCarro(double time){
    this.matrix = this.matrix * (((float)time - this.ultimotempo)* Matrix_Translate(1.0f,0.0f,0.0.f));
    this.ultimotempo. = time;
}
