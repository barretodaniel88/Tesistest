#include "cuerpo_h.h"

Cuerpo::Cuerpo()
{
}

Cuerpo::Cuerpo(float x, float y, float r )
{
    _PosX = x;
    _PosY = y;
    _Radio = r;
}

Cuerpo::~Cuerpo()
{
    //dtor
}

void Cuerpo::imprimir()
{
    cout << "\nEl cuerpo [" << _Numero << "] tiene:";
    cout << "\n\t" << "cx: " << _PosX;
    cout << "\n\t" << "cy: " << _PosY;
    cout << "\n\t" << "r: " << _Radio;
    cout << "\n";
}
