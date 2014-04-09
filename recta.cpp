#include "recta_h.h"

Recta::Recta()
{
    //ctor
}

Recta::~Recta()
{
    //dtor
}

void Recta::PuntoPendiente(float x, float y, double m)
{
    if(m>=3000)  _Pendiente = 3000;

}

