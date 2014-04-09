#ifndef SENSOR_H
#define SENSOR_H
#include "cuerpo_h.h"

class Agente;

class Sensor
{
    public:
        Sensor();
        Sensor(float angulo, float apertura, float alcance, int ncuerpos);
        virtual ~Sensor();
        int GetAgente() { return _Agente; }
        void SetAgente(int a) { _Agente = a; }
        float GetMedida() { return _Medida; }
        double GetAngulo() { return _Angulo; }
        void SetAngulo(double val) { _Angulo = val; }
        float GetApertura() { return _Apertura; }
        void SetApertura(float val) { _Apertura = val; }
        float GetAlcance() { return _Alcance; }
        void SetAlcance(float val) { _Alcance = val; }
        float GetMiraX() { return _MiraX; }
        void SetMiraX (float val) { _MiraX = val; }
        float GetMiraY() { return _MiraY; }
        void SetMiraY (float val) { _MiraY = val; }
        void SetArrCuerpos (Cuerpo *c, int n) { _cuerpos = c; _NCuerpos = n; }
        void SetArrAgentes (Cuerpo *c, int n) { _agentes = c; _NAgentes = n; }
        void Actualizar(float x0, float y0, float alfa, float radio);
        float SumAngulo(float tita);

    protected:
    private:
        float distancia2p(float x0, float y0, float x1, float y1);
        bool IntersectarRC(double m, double b, float h, float k, float r, float *r1, float *r2);
        bool CuerpoCentrado(float x, float y, float x1, float y1);
        float AnguloRelativo(float x0, float y0, float x1, float y1);
        float GetMin(float *array, int n);
        void SetMedida(float val) { if(val<_Medida) _Medida = val; };

        int _Agente;
        float _Medida;
        double _Angulo;
        float _Apertura;
        float _Alcance;
        float _MiraX;
        float _MiraY;
        int _NAgentes;
        int _NCuerpos;
        Cuerpo *_cuerpos;
        Cuerpo *_agentes;

        int temp;

};

#endif // SENSOR_H
