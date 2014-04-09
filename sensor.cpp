#include "sensor_h.h"
#include <iomanip>
#include <math.h>
#include <float.h>
#include <iostream>

//#define DEBUG

using namespace std;

Sensor::Sensor()
{
}

Sensor::Sensor(float angulo, float apertura, float alcance, int ncuerpos)
{
    _Angulo = angulo;
    _Apertura = apertura*M_PI/180;
    _Alcance = alcance;
    _NCuerpos = ncuerpos;
}

Sensor::~Sensor()
{
    //dtor
}

/*  Calcula la intersección entre un círculo C:(x-h)^2+(y-k)^2=r^2 y una recta L:y=mx+b
 *  Almacena ambos puntos como vectores <x,y> en los apuntadores *r1, *r2.
 *  Si hay intersección retorna TRUE, si no la hay retorna FALSE.
 */
bool Sensor::IntersectarRC(double m, double b, float h, float k, float r, float *r1, float *r2)
{
    double a,v,c;

/*  Definición de las constantes a,v,c para el cálculo de la resolvente */
    a = 1+pow(m,2);
    v = 2*(m*b-h-k*m);
    c = pow(b,2)+pow(h,2)+pow(k,2)-pow(r,2)-2*k*b;

/*  Información para hacer debugging */
    #ifdef DEBUG
        cout << "\n\nm: " << m << " | b: " << b;
        cout << "\na: " << a << " | v: " << v << "| c: " << c;
        cout << "\nr: " << r << " | h: " << h << "| k: " << k;
    #endif

/*  Si no hay intersección no modifica r1,r2 y retorna FALSE */
    if((pow(v,2)-4*a*c)<0)
        return false;

/*  Si hay intersección, guarda la información en r1,r2 y retorna TRUE */
    else
    {
        r1[0]= (-v+sqrt(pow(v,2)-4*a*c))/(2*a);
        r1[1]= m*r1[0] + b;
        r2[0]= (-v-sqrt(pow(v,2)-4*a*c))/(2*a);
        r2[1]= m*r2[0] + b;

        return true;
    }
}

/*  Calcula el modulo del ángulo formado entre el foco del sensor (x0,y0), la mira
 *  del sensor (_MiraX,_MiraY) y un punto cualquiera (x1,y1).
 */
float Sensor::AnguloRelativo(float x0, float y0, float x1, float y1)
{
/*  Implementa el teorema del coseno */
    double mod1 = sqrt(pow((double)(_MiraX-x0),2)+pow((double)(_MiraY-y0),2));
    double mod2 = sqrt(pow((double)(x1-x0),2)+pow((double)(y1-y0),2));
    double arg = ((_MiraX-x0)*(x1-x0)+(_MiraY-y0)*(y1-y0))/(mod1*mod2);
    float tita = acos(arg);

    return tita;
}

/*  Chequea si un cuerpo de coordenadas (x1,y1) tiene su centro dentro del área
 *  de cobertura del sensor.
 */
bool Sensor::CuerpoCentrado(float x0, float y0, float x1, float y1)
{
    if (AnguloRelativo(x0,y0,x1,y1)<(_Apertura/2)*M_PI/180)
        return true;
    else
        return false;
}

/*  Calcula la distancia entre dos puntos (x,y) en un valor float.
 *  NOTA: Mover a un archivo de utilidades disponibles en todo el proyecto.
 */
float Sensor::distancia2p(float x0, float y0, float x1, float y1)
{
    return sqrt(pow(x0-x1,2)+pow(y0-y1,2));
}

/*  Retorna el mínimo valor (con signo) encontrado en un arreglo de tipo float
 *  de longitud n.
 *  NOTA: Mover a un archivo de utilidades disponibles en todo el proyecto.
 */
float Sensor::GetMin(float *array, int n)
{
    float min = array[0];
    for(int i=1; i<n; i++)
    {
        if(array[i]<min)    min = array[i];
    }
    return min;
}


void Sensor::Actualizar(float x0, float y0, float alfa, float radio)
{
    double m1, b1, m2, b2;
    float c1[2], c2[2], angulo;

    _Medida = _Alcance;

/*  Calcula localmente el ángulo absoluto del sensor (hacia donde mira). */
    angulo = SumAngulo(alfa*M_PI/180);

/*  Actualiza la mira del sensor para los consecutivos calculos de AnguloRelativo */
    _MiraX = x0+radio*cos(angulo);
    _MiraY = y0+radio*sin(angulo);

/*  Calcula los parametros de las 2 rectas que limitan lateralmente el área del sensor */
/*  NOTA: Para evitar problemas por pérdida de información por precisión se podrían limitar los valores de m y b */
    m1 = tan(angulo-(_Apertura/2)*M_PI/180);
    b1 = y0 - m1*x0;
    m2 = tan(angulo+(_Apertura/2)*M_PI/180);
    b2 = y0 - m2*x0;

/*  Información de Debug */
    #ifdef DEBUG
        cout << "\ncuerpos: " << _NCuerpos;
    #endif

/*  Percibe los cuerpos (obstáculos) del escenario almacenados en el arreglo apuntado por *_cuerpos. */
    for (int i=0; i<_NCuerpos; i++)
    {
        float d = distancia2p(x0,y0,_cuerpos[i].GetPosX(),_cuerpos[i].GetPosY()) - _cuerpos[i].GetRadio() - radio;
        /*  Obvia todos los cuerpos que o estén fuera del radio de alcance del robot o esten fuera de cierto ángulo */
        if (d < _Alcance && AnguloRelativo(x0,y0,_cuerpos[i].GetPosX(),_cuerpos[i].GetPosY())<=(M_PI/4+_Apertura*M_PI/180))
        {
            #ifdef DEBUG
                cout << "\n\nPosicion obstaculo(" << i << "): [" << _cuerpos[i].GetPosX() << "," << _cuerpos[i].GetPosY() << "]";
            #endif
            if (CuerpoCentrado(x0,y0,_cuerpos[i].GetPosX(),_cuerpos[i].GetPosY()))
            {
                SetMedida(d);

                #ifdef DEBUG
                    cout << "\nCuerpo centrado: TRUE";
                #endif
            }
            else
            {
                #ifdef DEBUG
                    cout << "\nCuerpo centrado: FALSE";
                #endif

                float dv[4];
                if(IntersectarRC(m1,b1,_cuerpos[i].GetPosX(),_cuerpos[i].GetPosY(),_cuerpos[i].GetRadio(),c1,c2))
                {
                    #ifdef DEBUG
                        cout << "\nInterseccion 1: TRUE";
                    #endif

                    dv[0] = distancia2p(c1[0],c1[1],x0,y0) - radio;
                    dv[1] = distancia2p(c2[0],c2[1],x0,y0) - radio;

                    #ifdef DEBUG_MORE
                        cout << " | d(1): " << dv[0] << " | d(2): " << dv[1];
                        cout << "\nP1: (" << c1[0] << "," << c1[1] << ")";
                        cout << " | P2: (" << c2[0] << "," << c2[1] << ")";
                    #endif
                }
                else
                {
                    dv[0] = dv[1] =  _Alcance;
                    #ifdef DEBUG
                        cout << "\nInterseccion 1: FALSE";
                    #endif
                }
                if(IntersectarRC(m2,b2,_cuerpos[i].GetPosX(),_cuerpos[i].GetPosY(),_cuerpos[i].GetRadio(),c1,c2))
                {
                    #ifdef DEBUG
                        cout << "\nInterseccion 2: TRUE";
                    #endif

                    dv[2] = distancia2p(c1[0],c1[1],x0,y0) - radio;
                    dv[3] = distancia2p(c2[0],c2[1],x0,y0) - radio;

                    #ifdef DEBUG_MORE
                        cout << " | d(1): " << dv[2] << " | d(2): " << dv[3];
                        cout << "\nP1: (" << c1[0] << "," << c1[1] << ")";
                        cout << " | P2: (" << c2[0] << "," << c2[1] << ")";
                    #endif
                }
                else
                {
                    dv[2] = dv[3] = _Alcance;
                    #ifdef DEBUG
                        cout << "\nInterseccion 2: FALSE";
                    #endif
                }

                SetMedida(GetMin(dv,4));
            }
        }
    }

    for (int i=0; i<_NAgentes; i++)
    {
        if(_agentes[i].GetNumero()==_Agente){}
        else
        {
            float d = distancia2p(x0,y0,_agentes[i].GetPosX(),_agentes[i].GetPosY()) - _agentes[i].GetRadio() - radio;
            if (d <= _Alcance && AnguloRelativo(x0,y0,_agentes[i].GetPosX(),_agentes[i].GetPosY())<=(M_PI/4+_Apertura*M_PI/180))
            {
                if (CuerpoCentrado(x0,y0,_agentes[i].GetPosX(),_agentes[i].GetPosY()))
                {
                    SetMedida(d);
                }
                else
                {
                    float dv[4];
                    if(IntersectarRC(m1,b1,_agentes[i].GetPosX(),_agentes[i].GetPosY(),_agentes[i].GetRadio(),c1,c2))
                    {
                        dv[0] = distancia2p(c1[0],c1[1],x0,y0) - radio;
                        dv[1] = distancia2p(c2[0],c2[1],x0,y0) - radio;
                    }
                    else
                    {
                        dv[0] = dv[1] =  _Alcance;
                    }
                    if(IntersectarRC(m2,b2,_agentes[i].GetPosX(),_agentes[i].GetPosY(),_agentes[i].GetRadio(),c1,c2))
                    {
                        dv[2] = distancia2p(c1[0],c1[1],x0,y0) - radio;
                        dv[3] = distancia2p(c2[0],c2[1],x0,y0) - radio;
                    }
                    else
                    {
                        dv[2] = dv[3] = _Alcance;
                    }

                    SetMedida(GetMin(dv,4));
                }
            }
        }
    }
    if(_Medida == FLT_MAX)  _Medida = _Alcance;
}

float Sensor::SumAngulo(float tita)
{
    float alfa = _Angulo + tita;
    if(alfa>2*M_PI)   alfa-=2*M_PI;
    else if(alfa<0)   alfa+=2*M_PI;
    return alfa;
}
