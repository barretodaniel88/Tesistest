#include "agente_h.h"


Agente::Agente()
{

}

Agente::Agente(float x, float y, float r,
               int nsensores, float apertura, float alcance)
            : Cuerpo( x, y, r)
{
    _Angulo = 0;
    _VelDer = 0;
    _VelIzq = 0;
    _Delta = 0.05;
    _numSensores = nsensores;

    double angulo = -(apertura*(nsensores-1)/2)*M_PI/180;

    for(int i=0; i<nsensores; i++)
    {
        _Sensores[i].SetAngulo(angulo);
        angulo += apertura*M_PI/180;
        _Sensores[i].SetApertura(apertura);
        _Sensores[i].SetAlcance(alcance);
    }
}

Agente::~Agente()
{
    //dtor
}

void Agente::InitSensores(int nsensores, float apertura, float alcance)
{
    _numSensores = nsensores;
    double angulo = -(apertura*(nsensores-1)/2)*M_PI/180;

    for(int i=0; i<nsensores; i++)
    {
        _Sensores[i].SetAgente(_Numero);
        _Sensores[i].SetAngulo(angulo);
        angulo += apertura*M_PI/180;
        _Sensores[i].SetApertura(apertura);
        _Sensores[i].SetAlcance(alcance);
    }
}

void Agente::SetArrObstaculos(Cuerpo *apobstaculos, int nobstaculos)
{
    _apObstaculos = apobstaculos;
    _numObstaculos = nobstaculos;
    for(int i=0; i<_numSensores; i++)
    {
        _Sensores[i].SetArrCuerpos(apobstaculos,nobstaculos);
//        _Sensores[i].Actualizar(_PosX,_PosY,_Angulo,_Radio);
    }
}

void Agente::SetArrAgentes(Cuerpo *apagentes, int nagentes)
{
    _apAgentes = apagentes;
    _numAgentes = nagentes;
    for(int i=0; i<_numSensores; i++)
    {
        _Sensores[i].SetArrAgentes(apagentes, nagentes);
    }
}

void Agente::Iterar()
{
    //recalculo todas las variables de estado del robot para 1 sola iteracion
    register float w = GetVelAngular();
    register float v = GetVelLineal();
    float tempx, tempy;
    //luego calculo el nuevo angulo de rotacion
    _Angulo = sumAngulo(_Delta*w);
    _MiraX = _PosX+5*cos(_Angulo);
    _MiraY = _PosY+5*sin(_Angulo);

    //ahora calculo los nuevos valores de coordenadas X y Y a partir de la velocidad lineal (v)
    tempx = _PosX;
    tempy = _PosY;
    _PosX = _PosX + v*cos(_Angulo)*_Delta;
    _PosY = _PosY + v*sin(_Angulo)*_Delta;

    if(DetectarColisiones())
    {
        _PosX = tempx;
        _PosY = tempy;
    }
    if(_Numero==0)
    {
        cout << "\n\nColisiones: " << _numColisiones << " [";
        for(int i=0; i<_numColisiones; i++)
            cout << _Colisiones[i] << ",";
        cout << "]";
    }
    DetectarColisiones();
    if(_Numero==0)
    {
        cout << "\n\nColisiones: " << _numColisiones << " [";
        for(int i=0; i<_numColisiones; i++)
            cout << _Colisiones[i] << ",";
        cout << "]";
    }



        for(int i=0; i<_numSensores; i++)
        {
            _Sensores[i].SumAngulo(_Delta*w);
            _Sensores[i].Actualizar(_PosX,_PosY,_Angulo*180/M_PI,_Radio);
        }
}

bool Agente::DetectarColisiones()
{
    int d;
    _numColisiones = 0;
    for (int i=0; i<_numAgentes; i++)
    {
    /*  No hacer nada si se trata de si mismo */
        if (_apAgentes[i].GetNumero()==_Numero){}
        else
        {
        /*  Medir la distancia entre los centros */
            d = Distancia2p(_PosX,_PosY,_apAgentes[i].GetPosX(),_apAgentes[i].GetPosY());
            d = d - (_Radio + _apAgentes[i].GetRadio());
            if (d <= 0)
            {
                _Colisiones[_numColisiones] = AnguloRelativo(_PosX, _PosY,_apAgentes[i].GetPosX(),_apAgentes[i].GetPosY());
                _numColisiones++;
            }
        }
    }
/*  Detectar colisiones con obstaculos */
    for (int i=0; i<_numObstaculos; i++)
    {
        d = Distancia2p(_PosX,_PosY,_apObstaculos[i].GetPosX(),_apObstaculos[i].GetPosY());
        d = d - (_Radio + _apObstaculos[i].GetRadio());
        if (d <= 0)
        {
            _Colisiones[_numColisiones] = AnguloRelativo(_PosX, _PosY, _apObstaculos[i].GetPosX(),_apObstaculos[i].GetPosY());
            _numColisiones++;
        }
    }
    if (_numColisiones>0)   return true;
    else                    return false;
}

float Agente::sumAngulo(float tita)
{
    float alfa = _Angulo + tita;
    if(alfa>2*M_PI)   alfa-=2*M_PI;
    else if(alfa<0)   alfa+=2*M_PI;
    return alfa;
}

float Agente::AnguloRelativo(float x0, float y0, float x1, float y1)
{
    //calcular el angulo de 12 a 13
    float mod1 = sqrt(pow((_MiraX-x0),2)+pow((_MiraY-y0),2));
    float mod2 = sqrt(pow((x1-x0),2)+pow((y1-y0),2));
    float tita;
    tita = acos(((_MiraX-x0)*(x1-x0)+(_MiraY-y0)*(y1-y0))/(mod1*mod2));
    return tita;
}

float Agente::Distancia2p(float x0, float y0, float x1, float y1)
{
    return sqrt(pow(x0-x1,2)+pow(y0-y1,2));
}

void Agente::imprimir()
{
    cout << "\nEl agente [" << _Numero << "] tiene:";
    cout << "\n\t" << "cx: " << _PosX;
    cout << "\n\t" << "cy: " << _PosY;
    cout << "\n\t" << "r: " << _Radio;
    cout << "\n\t" << "angulo: " << _Angulo;
    cout << "\n\t" << "sensores:" << _numSensores;
    cout << "\n";
}
