#ifndef AGENTE_H
#define AGENTE_H


#include "sensor_h.h"

#include <math.h>
#include <iostream>

#define MAX_SEN     20
#define MAX_CRP     100
#define MAX_COL     200

using namespace std;

class Sensor;

class Agente : public Cuerpo
{
    public:
        Agente();
        Agente(float x, float y, float r, int nsensores, float apertura, float alcance);
        virtual ~Agente();
        void InitSensores(int nsensores, float apertura, float alcance);

        double GetAngulo() { return _Angulo; }
        void SetAngulo(double val) { _Angulo = val; }
        float GetVelDer() { return _VelDer; }
        void SetVelDer(float val) { _VelDer = val; }
        float GetVelIzq() { return _VelIzq; }
        void SetVelIzq(float val) { _VelIzq = val; }
        float GetVelLineal() { return ((_VelDer + _VelIzq)/2); }
        float GetVelAngular() { return ((_VelDer - _VelIzq)/(2*_Radio)); }
        float GetVelX() { return this->GetVelLineal()*cos(_Angulo); }
        float GetVelY() { return this->GetVelLineal()*sin(_Angulo); }
        float GetMiraX() { return _MiraX; }
        float GetMiraY() { return _MiraY; }
        float GetDelta() { return _Delta; }
        void SetDelta(float val) { _Delta = val; }
        int GetNColisiones(void) { return _numColisiones; }
        int GetNSensores(void) { return _numSensores;}
        void SetArrAgentes(Cuerpo *apagentes, int nagentes);
        void SetArrObstaculos(Cuerpo *apobstaculos, int nobstaculos);
        void Iterar(void); //funcion que simula una iteracion de movimiento del robot
        void Iterar(unsigned int N); //funcion que simula N iteraciones de movimiento del robot
        Sensor _Sensores[MAX_SEN];
        Cuerpo *_apObstaculos;
        Cuerpo *_apAgentes;
        float sumAngulo(float tita);
        bool DetectarColisiones();
        float AnguloRelativo(float x0, float y0, float x1, float y1);
        float Distancia2p(float x0, float y0, float x1, float y1);
        float _Colisiones[MAX_CRP];
        void imprimir();

    protected:
    private:
        //int _Numero;
        double _Angulo;
        float _VelDer;
        float _VelIzq;
        float _MiraX;
        float _MiraY;
        float _Delta;


        int _numSensores;
        int _numObstaculos;
        int _numColisiones;
        int _numAgentes;
};

#endif // AGENTE_H
