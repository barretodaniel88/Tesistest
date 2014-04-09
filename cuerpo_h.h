#ifndef CUERPO_H
#define CUERPO_H

#include <iostream>

using namespace std;

class Cuerpo
{
    public:
        Cuerpo();
        Cuerpo(float x, float y, float r );
        virtual ~Cuerpo();
        int GetNumero() { return _Numero; }
        void SetNumero(int n) { _Numero = n; }
        float GetPosX() { return _PosX; }
        void SetPosX(float val) { _PosX = val; }
        float GetPosY() { return _PosY; }
        void SetPosY(float val) { _PosY = val; }
        float GetRadio() { return _Radio; }
        void SetRadio(float val) { _Radio = val; }
        void imprimir();
    protected:
        int _Numero;
        float _PosX;
        float _PosY;
        float _Radio;
    private:

};

#endif // CUERPO_H
