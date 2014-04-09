#ifndef RECTA_H
#define RECTA_H


class Recta
{
    public:
        Recta();
        virtual ~Recta();
        double GetPendiente() { return _Pendiente; }
        double GetCorteY() { return _CorteY; }
        double GetCorteX() { return _CorteX; }
        void PuntoPendiente(float x, float y, double m);
        void DosPuntos(float x1, float y1, float x2, float y2);
    protected:
    private:
        double _Pendiente;
        double _CorteY;
        double _CorteX;
        bool _Vertical;
};

#endif // RECTA_H
