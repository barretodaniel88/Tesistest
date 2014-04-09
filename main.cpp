#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <iomanip>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#include <string>
#include <cstdlib>
#include <sstream>

#define DEFAULT_PORT 10000
#define DEFAULT_BUFLEN 2500

#include "agente_h.h"
#include <GL/freeglut.h>
#include <GL/gl.h>

#define MAX_OBS     100
#define DIBUJAR
//#define SIMULAR
//#define DEBUG

using namespace std;

void init(void);
void reshape(int w, int h);
void idle(void);
void keyboard(unsigned char key, int x, int y);
void display(void);
void DibujarSensor(void);
void InitObstaculos(int n);
void DibujarMapa(float res);
void Agentes2Cuerpos();
void DibujarAgentes();
void InitAgentes();

//void myConnect();
void initSock();
void listenSock();
string getBodyByPos(string, int);
void JSON2Cuerpo(string, int);
void JSON2Agente(string, int);
void getAllObjects(string, Cuerpo *, Agente *);
int buildJSONpackage(char *, Cuerpo *, Agente *);
string cuerpo2JSON(Cuerpo);
string agente2JSON(Agente);
string JSONgetString(string, string);
float JSONgetNum(string, string);

char recvbuf[DEFAULT_BUFLEN], sendbuf[DEFAULT_BUFLEN] = "Hola desde el planeta C";
int iResult, iSendResult;
int recvbuflen = DEFAULT_BUFLEN;
WSADATA wsaData;
sockaddr_in local;
SOCKET server, client;

int w_mapa = 600;
int h_mapa = 600;
Agente AGENTES[100];
int nagentes = 0;
Cuerpo CUERPOSAGENTES[100];
Cuerpo CUERPOS[100];
bool pausa=false;
bool slow=false;
int t=0;
float alfa=0;

int nobstaculos=0;
float slices=1.6;
fstream archivo;


GLfloat color_blanco[4] = {1.0,1.0,1.0,1.0};
GLfloat color_suelo[4] = {0.4,0.8,0.8,1.0};
GLfloat color_grid[4] = {0.3,0.7,0.7,0.6};
GLfloat color_objetos[4] = {1.0,0.2,0.2,1.0};
GLfloat color_agentes[4] = {1.0,1.0,1.0,1.0};
GLfloat color_sensores[4] = {0.0,0.8,0.0,0.5};
GLfloat color_mira[4] = {0.0,0.5,0.0,1.0};
GLfloat color_colision[4] = {0.0,0.0,0.0,1.0};
GLfloat color_seleccion[4] = {8.0,4.0,0.0,1.0};

int main(int argc, char **argv)
{
#ifdef DEBUG
nagentes = 6;
nobstaculos = 4;
#endif
    //myConnect()
#ifdef DIBUJAR
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(400,100);
    glutCreateWindow("PRUEBA SENSOR");
#endif
    init();
#ifdef DIBUJAR
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMainLoop();
#endif
    return 0;
}

void init(void)
{
    srand(time(NULL));
//    srand(21751);

//    glBlendColor(0.5,0.5,0.5,1.0);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_ONE, GL_ZERO );
#ifndef DEBUG
    initSock();
    listenSock();
#endif


    cout << "\n\nNOBSTACULOS: " << nobstaculos;
    cout << "\nNAGENTES: " << nagentes;
#ifdef DEBUG
    InitObstaculos(nobstaculos);
    InitAgentes();
#endif
    archivo.open("sensor.xls",  fstream::out);
    Agentes2Cuerpos();
}

void reshape(int w, int h)
{
    if(w>h)
        glViewport((w-h)/2,0, (GLsizei) h, (GLsizei) h);
    else
        glViewport(0,(h-w)/2, (GLsizei) w, (GLsizei) w);

/*  Define el tipo de proyección: orthographic (sin perspectiva) */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluOrtho2D(-w_mapa/2,w_mapa/2,-h_mapa/2,h_mapa/2);
    gluOrtho2D(0,w_mapa,0,h_mapa);
}

void idle(void)
{
//    if(t<3000){
    if(slow) pausa=false;
    if(!pausa)
    {
        t++;
        cout << "\n\nt: " << t;
//        cout << "\n\n   " <<glGetString(GL_RENDERER);
        Agentes2Cuerpos();
        for(int i=0; i<nagentes; i++)   AGENTES[i].Iterar();
//        archivo << "AGENTE[0]: ";
        for(int i=0; i<AGENTES[0].GetNSensores(); i++)
        {
            archivo << AGENTES[0]._Sensores[i].GetMedida() << "\t";
        }
        archivo << "\n";
//        if(t==656)  AGENTES[0]._Sensores[8]._DebugInfo = true;
//        if(t>1000 && t<1200)
//        {
//            slow = true;
//            cout << "\nMedida: " << AGENTES[0]._Sensores[1].GetMedida();
//            cout << "\nMira: [" << AGENTES[0]._Sensores[1].GetMiraX() << "," <<AGENTES[0]._Sensores[1].GetMiraY() << "]";
//            cout << "\nMi posicion: [" << AGENTES[0].GetPosX() << "," << AGENTES[0].GetPosY() << "]";
//            AGENTES[0]._Sensores[1]._DebugInfo = true;
////
//        }
        if(slow)   { pausa=true; slow =!slow;}
//    }
    }
//    if(t>600){
//    AGENTES[0]._Sensores[8]._DebugInfo = true;
#ifndef SIMULAR
    pausa = true;
#endif
    listenSock();
    glutPostRedisplay();

//    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27: exit(EXIT_SUCCESS);
                 archivo.close();
        break;
        case 'p': pausa=!pausa;
        break;
        case 's': slow=!slow;
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    DibujarMapa(10);
    DibujarAgentes();

    glColor3f(1.0,0.0,0.0);
    for(int i=0; i<nobstaculos; i++)
    {
        glPushMatrix();
        glTranslatef(CUERPOS[i].GetPosX(),CUERPOS[i].GetPosY(),0.0);
        glutSolidCylinder(CUERPOS[i].GetRadio(),0.1,(int)slices*CUERPOS[i].GetRadio(),1);
        glPopMatrix();
    }

    glutSwapBuffers();
}

void DibujarSensor(void)
{
//    int s = slices*(180/)
}

void InitAgentes()
{
    for(int i=0; i<nagentes; i++)
    {
        Agentes2Cuerpos();
        AGENTES[i].SetRadio(rand()%10+10);
        AGENTES[i].SetAngulo((rand()%360)*M_PI/180);
        AGENTES[i].InitSensores(9,10,100);
        AGENTES[i].SetArrObstaculos(CUERPOS,nobstaculos);
        AGENTES[i].SetArrAgentes(CUERPOSAGENTES,nagentes);
        AGENTES[i].SetNumero(i);

        do
        {
            /*
            AGENTES[i].SetPosX(rand()%(int)(w_mapa/2-AGENTES[i].GetRadio())-w_mapa/4);
            AGENTES[i].SetPosY(rand()%(int)(h_mapa/2-AGENTES[i].GetRadio())-h_mapa/4);
            */
            AGENTES[i].SetPosX(rand()%(int)(w_mapa));
            AGENTES[i].SetPosY(rand()%(int)(h_mapa));
        }while(AGENTES[i].DetectarColisiones());

        AGENTES[i].InitSensores(9,10,100);
        AGENTES[i].SetVelDer(13.0);
        AGENTES[i].SetVelIzq(10.0);
        AGENTES[i].SetDelta(0.05);


        AGENTES[i].Iterar();
    }
}
void InitObstaculos(int n)
{
    for(int i=0; i<nobstaculos; i++)
    {
        CUERPOS[i].SetRadio(10);
        /*
        CUERPOS[i].SetPosX(rand()%(int)(w_mapa-2*CUERPOS[i].GetRadio())-w_mapa/2+CUERPOS[i].GetRadio());
        CUERPOS[i].SetPosY(rand()%(int)(h_mapa-2*CUERPOS[i].GetRadio())-h_mapa/2+CUERPOS[i].GetRadio());
        */
        CUERPOS[i].SetPosX(rand()%(int)(w_mapa));
        CUERPOS[i].SetPosY(rand()%(int)(h_mapa));
    }
}

void DibujarMapa(float res)
{
    glColor4fv(color_grid);

/*  Dibuja las lineas horizontales */
    glPushMatrix();
    glTranslatef(0,0,-0.1);
    for(int i=0; i<=h_mapa; i+=res)
    {
        glBegin(GL_LINES);
            glVertex2f(0.0,0.0);
            glVertex2f(w_mapa,0.0);
        glEnd();
        glTranslatef(0.0,res,0.0);
    }
    glPopMatrix();

/*  Dibuja las lineas verticales */
    glPushMatrix();
    glTranslatef(0,0,-0.1);
    for(int i=0; i<=w_mapa; i+=res)
    {
        glBegin(GL_LINES);
            glVertex2f(0.0,0.0);
            glVertex2f(0.0,h_mapa);
        glEnd();
        glTranslatef(res,0.0,0.0);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0,0.0,-0.1);
    glColor4fv(color_suelo);
    glRectf(0,0,w_mapa,h_mapa);
    glPopMatrix();
}

void DibujarAgentes()
{
    for(int i=0; i<nagentes; i++)
    {

        glPushMatrix();

        glTranslatef(AGENTES[i].GetPosX(), AGENTES[i].GetPosY(),0.0);
        glColor4fv(color_agentes);
        if(i==0)    glColor4fv(color_seleccion);
        glutSolidCylinder(AGENTES[i].GetRadio(),0.1,32,1);
        glRotatef(AGENTES[i].GetAngulo()*180/M_PI,0.0,0.0,1.0);


        for(int j=0; j<AGENTES[i].GetNColisiones(); j++)
        {
            glPushMatrix();
            glTranslatef(0.0,0.0,0.9);
            glRotatef(AGENTES[i]._Colisiones[j]*180/M_PI,0.0,0.0,1.0);
            glColor4fv(color_colision);
            glPointSize(4);
            glBegin(GL_POINTS);
                glVertex2f(AGENTES[i].GetRadio()-1,0.0);
            glEnd();
            glPopMatrix();
        }




        glPushMatrix();
        glTranslatef(0.0,0.0,0.2);
        glColor4fv(color_objetos);
        glBegin(GL_LINES);
            glVertex2f(AGENTES[i].GetRadio(),0.0);
            glVertex2f(0.0,0.0);
        glEnd();
        glPopMatrix();
//        cout << "\nsensores: " << AGENTES[i].GetNSensores();
//        glBlendColor(0.5,0.5,0.5,1.0);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

        for(int j=0; j<AGENTES[i].GetNSensores(); j++)
        {
//            cout << "\nMiraX: " << AGENTES[i]._Sensores[j].GetMiraX() << " MiraY: " << AGENTES[i]._Sensores[j].GetMiraY();
            glPushMatrix();
            glLoadIdentity();
            glColor4fv(color_mira);
            glPointSize(4);
            glBegin(GL_POINTS);
                glVertex2f(AGENTES[i]._Sensores[j].GetMiraX(),AGENTES[i]._Sensores[j].GetMiraY());
            glEnd();
            glPopMatrix();

            glPushMatrix();
            glRotatef(AGENTES[i]._Sensores[j].GetAngulo()*180/M_PI,0.0,0.0,1.0);
            glRotatef(-AGENTES[i]._Sensores[j].GetApertura()/2,0.0,0.0,1.0);
//            glColor4fv(color_sensores);
            glColor4f(1.0-(AGENTES[i]._Sensores[j].GetMedida()/AGENTES[i]._Sensores[j].GetAlcance()),(AGENTES[i]._Sensores[j].GetMedida()/AGENTES[i]._Sensores[j].GetAlcance()),0.0,0.8);
            glBegin(GL_TRIANGLE_FAN);
                glVertex2f(0,0);
                glVertex2f(AGENTES[i].GetRadio()+AGENTES[i]._Sensores[j].GetMedida(),0);
                glVertex2f((AGENTES[i].GetRadio()+AGENTES[i]._Sensores[j].GetMedida())*cos(AGENTES[i]._Sensores[j].GetApertura()*M_PI/180),(AGENTES[i].GetRadio()+AGENTES[i]._Sensores[j].GetMedida())*sin(AGENTES[i]._Sensores[j].GetApertura()*M_PI/180));
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
        glBlendFunc(GL_ONE, GL_ZERO);

    }
}

void Agentes2Cuerpos()
{
    for(int i=0; i<nagentes; i++)
    {
        CUERPOSAGENTES[i].SetPosX(AGENTES[i].GetPosX());
        CUERPOSAGENTES[i].SetPosY(AGENTES[i].GetPosY());
        CUERPOSAGENTES[i].SetRadio(AGENTES[i].GetRadio());
        CUERPOSAGENTES[i].SetNumero(AGENTES[i].GetNumero());
    }
}

void initSock()
{
        int iResult;


        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            cout << "WSAStartup failed: " << iResult << "\n";
            return;
        }else
        {
            cout << "WSAStartup correct\n";
        }

        server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(server == INVALID_SOCKET)
        {
            cout << "Error creating socket\n";
            closesocket(server);
            return;
        }
        else
        {
            // Si se creó correctamente el socket del servidor
            //ZeroMemory(&local, sizeof (local));
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = INADDR_ANY;
            local.sin_port = htons(DEFAULT_PORT);

            if( bind(server, (sockaddr *)&local, sizeof(local)) == SOCKET_ERROR )
            {
                cout << "Error binding socket\n";
                closesocket(server);
                return;
            }
            else{cout << "Binding OK\n";}
        }
}
void listenSock(){
    char temp[DEFAULT_BUFLEN];
    if (listen(server,1) == SOCKET_ERROR)
            {
                cout << "Error listening on socket\n";
                closesocket(server);
            }
            else
            {
                // Si está escuchando...
                cout << "I'm Listening...\n";
                client = INVALID_SOCKET;
                client = accept(server,NULL,NULL);
                if (client == INVALID_SOCKET) {
                    printf("accept failed: %d\n", WSAGetLastError());
                    closesocket(server);
                    //WSACleanup();
                    //return 1;
                }
                cout << "Conectado\n";
                iResult = 0;
                do {
                    iResult = recv(client, recvbuf, recvbuflen, 0);
                    if (iResult > 0) {
                        printf("Bytes received: %d\n", iResult);
                        getAllObjects(recvbuf, CUERPOS, AGENTES);
                        /*
                        for(int i=0; i<nobstaculos; i++)
                        {
                            CUERPOS[i].SetPosX(CUERPOS[i].GetPosX()+30);
                            AGENTES[i].SetPosY(AGENTES[i].GetPosY()+30);
                        }*/
                        int len = buildJSONpackage(temp,CUERPOS, AGENTES);
                        // Echo the buffer back to the sender
                        iSendResult = send(client, temp, len, 0);
                        if (iSendResult == SOCKET_ERROR) {
                            printf("send failed: %d\n", WSAGetLastError());
                            closesocket(client);
                            //WSACleanup();
                            //return 1;
                        }
                        printf("Bytes sent: %d\n", iSendResult);
                        closesocket(client);
                        //WSACleanup();
                        //return 1;
                    } else if (iResult == 0)
                        printf("Connection closing...\n");
                    else {
                        printf("recv failed: %d\n", WSAGetLastError());
                        //closesocket(client);
                        //WSACleanup();
                        cout << iResult;
                        //return 1;
                    }
                } while (iResult > 0);
                //for(;;){}
                cout << "FIN\n";
                //closesocket(client);
                //WSACleanup();
            }
}
/*
void myConnect()
{
    int iResult;
    char temp[DEFAULT_BUFLEN];

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed: " << iResult << "\n";
        return;
    }else
    {
        cout << "WSAStartup correct\n";
    }

    server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(server == INVALID_SOCKET)
    {
        cout << "Error creating socket\n";
        closesocket(server);
        return;
    }
    else
    {
        // Si se creó correctamente el socket del servidor
        //ZeroMemory(&local, sizeof (local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(DEFAULT_PORT);

        if( bind(server, (sockaddr *)&local, sizeof(local)) == SOCKET_ERROR )
        {
            cout << "Error binding socket\n";
            closesocket(server);
            return;
        }
        else
        {

            // si se logró el binding con la dirección local
            cout << "Binding OK\n";
            if (listen(server,1) == SOCKET_ERROR)
            {
                cout << "Error listening on socket\n";
                closesocket(server);
            }
            else
            {
                // Si está escuchando...
                cout << "I'm Listening...\n";
                client = INVALID_SOCKET;
                client = accept(server,NULL,NULL);
                if (client == INVALID_SOCKET) {
                    printf("accept failed: %d\n", WSAGetLastError());
                    closesocket(server);
                    //WSACleanup();
                    //return 1;
                }
                cout << "Conectado\n";
                iResult = 0;
                do {
                    iResult = recv(client, recvbuf, recvbuflen, 0);
                    if (iResult > 0) {
                        printf("Bytes received: %d\n", iResult);
                        getAllObjects(recvbuf, CUERPOS, AGENTES);
                        for(int i=0; i<nobstaculos; i++)
                        {
                            CUERPOS[i].SetPosX(CUERPOS[i].GetPosX()+30);
                            AGENTES[i].SetPosY(AGENTES[i].GetPosY()+30);
                        }
                        int len = buildJSONpackage(temp,CUERPOS, AGENTES);
                        // Echo the buffer back to the sender
                        iSendResult = send(client, temp, len, 0);
                        if (iSendResult == SOCKET_ERROR) {
                            printf("send failed: %d\n", WSAGetLastError());
                            closesocket(client);
                            //WSACleanup();
                            //return 1;
                        }
                        printf("Bytes sent: %d\n", iSendResult);
                        closesocket(client);
                        //WSACleanup();
                        //return 1;
                    } else if (iResult == 0)
                        printf("Connection closing...\n");
                    else {
                        printf("recv failed: %d\n", WSAGetLastError());
                        //closesocket(client);
                        //WSACleanup();
                        cout << iResult;
                        //return 1;
                    }
                } while (iResult > 0);
                //for(;;){}
                cout << "FIN\n";
                //closesocket(client);
                //WSACleanup();
            }
        }
    }
}
*/
string getBodyByPos(string jsonstring, int p)
{
    string obs, temp;
    size_t found, begin, end, length;
    int i;

    found = jsonstring.find("objects");
    temp = "objects\":[";
    found += temp.length();
    cout << found <<"\n";

        for(i=0; i<p; i++)
        {
            //cout << "\tdentro del bucle " << i << "\n";
            found = jsonstring.find("},{",found);
            found += 3;
        }
    begin = found+1;
    end = jsonstring.find("}",begin);
    length = end - begin;

    obs = jsonstring.substr(begin, length);
    cout << obs << "\n";

    return obs;
}

void JSON2Cuerpo(string c, int i)
{
    CUERPOS[i].SetNumero((int)JSONgetNum(c, "num"));
    CUERPOS[i].SetRadio(JSONgetNum(c, "r"));
    CUERPOS[i].SetPosX(JSONgetNum(c, "cx"));
    CUERPOS[i].SetPosY(JSONgetNum(c, "cy"));
    CUERPOS[i].imprimir();
/*
    float cx, cy, r;

    cx = JSONgetNum(c, "cx");
    cy = JSONgetNum(c, "cy");
    r = JSONgetNum(c, "r");

    Cuerpo mycuerpo(cx,cy,r);
    cout << "El cuerpo tiene: \n" << "\tcx: " << cx << "\n\tcy: " << cy << "\n\tr: " << r << "\n";
    return mycuerpo;
    */
}

void JSON2Agente(string c, int i)
{

    cout << "num:" <<JSONgetNum(c, "num") << "\n";

    //Agentes2Cuerpos();
    AGENTES[i].SetNumero((int)JSONgetNum(c, "num"));
    AGENTES[i].SetRadio(JSONgetNum(c, "r"));
    AGENTES[i].SetAngulo(JSONgetNum(c, "angle"));
    AGENTES[i].InitSensores(9,10,100);
    AGENTES[i].SetArrObstaculos(CUERPOS,nobstaculos);
    AGENTES[i].SetArrAgentes(CUERPOSAGENTES,nagentes);


    AGENTES[i].SetPosX(JSONgetNum(c, "cx"));
    AGENTES[i].SetPosY(JSONgetNum(c, "cy"));

    AGENTES[i].InitSensores(9,10,100);
    AGENTES[i].SetVelDer(13.0);
    AGENTES[i].SetVelIzq(10.0);
    AGENTES[i].SetDelta(0.05);

    AGENTES[i].Iterar();
    Agentes2Cuerpos();
    AGENTES[i].imprimir();

}

void getAllObjects(string jsonstring, Cuerpo *c, Agente *a)
{
    //int nagentes = 0, nobstaculos = 0;
    int length = (int)JSONgetNum(jsonstring, "long");
    int cobs = 0, cags = 0;

    cout << jsonstring;
    nagentes = JSONgetNum(jsonstring, "nagentes");
    nobstaculos = length - nagentes;
    string temp;
    for(int i=0; i<length; i++)
    {
        temp = getBodyByPos(jsonstring,i);

        cout << "\nNOBSTACULOS: " << nobstaculos << "\n";
        cout << "NAGENTES: " << nagentes << "\n";

        if(temp.find("angle") == string::npos)
        {
            cout << "El cuerpo de la posicion " << i << " es obstaculo\n";
            JSON2Cuerpo(temp, cobs);
            //c[nobstaculos] = JSON2Cuerpo(temp);
            //nobstaculos++;
            cobs++;

        }else{
            cout << "El cuerpo de la posicion " << i << " es agente\n";
            JSON2Agente(temp, cags);
            //a[nagentes] = JSON2Agente(temp);
            //nagentes++;
            cags++;
        }
    }

    cout << "El paquete tiene " << length << " cuerpos\n";
    cout << "\tnumero de agentes " << nagentes << "\n";
    cout << "\tnumero de obstaculos " << nobstaculos << "\n";
    cout << "------ AQUI -----\n";
    for(int i=0; i<nagentes; i++)
    {
        CUERPOSAGENTES[i].imprimir();
    }
}

string JSONgetString(string jsonstring, string name)
{
    string value;
    size_t found, begin, end, length;

    found = jsonstring.find(name);
    name = name + "\":\"";
    begin = found + name.length();

    end = jsonstring.find("\"",begin);
    length = end - begin;

    value = jsonstring.substr(begin,length);
    return value;
}

float JSONgetNum(string jsonstring, string name)
{
    string value;
    size_t found, begin, end, length;

    found = jsonstring.find(name);
    name = name + "\":";
    begin = found + name.length();

    end = jsonstring.find_first_of(",}",begin);
    length = end - begin;

    //cout << "begin: " << begin << " end: " << end << "\n";

    value = jsonstring.substr(begin,length);
    return atof(value.c_str());
}

/* /////////////////////////////////////////////////////////////// */

int buildJSONpackage(char *result, Cuerpo *c, Agente *a)
{
    string jsonstring = "{\"config\":{\"tipo\":\"final\",\"long\":";
    stringstream temp;
    temp << nobstaculos+nagentes;
    jsonstring += temp.str();
    jsonstring += "},\"objects\":[";
    cout << "nobstaculos: " << nobstaculos;
    for(int i=0; i< nobstaculos; i++){
        cout << "entre";
        jsonstring += cuerpo2JSON(c[i]) + ",";
    }
    for(int i=0; i< nagentes; i++){
        jsonstring += agente2JSON(a[i]);
        if(i<nagentes-1)    jsonstring += ",";
    }
    jsonstring += "]}";
    cout << jsonstring.length();
    cout << "\n\tRESPUESTA: " <<(char *)jsonstring.c_str() << "\n";
    strcpy(result, (char *)jsonstring.c_str());
    //return (char *)jsonstring.c_str();
    //return jsonstring;
    return jsonstring.length();
}
string cuerpo2JSON(Cuerpo c)
{
    string JSON;
    stringstream cx, cy, r;
    cx << c.GetPosX();
    JSON += "{\"cx\":" + cx.str() + ",";
    cy << c.GetPosY();
    JSON += "\"cy\":" + cy.str() + ",";
    r << c.GetRadio();
    JSON += "\"r\":" + r.str() + "}";
    return JSON;
}

string agente2JSON(Agente a)
{
    string JSON;
    stringstream cx, cy, r, angle;
    cx << a.GetPosX();
    JSON += "{\"cx\":" + cx.str() + ",";
    cy << a.GetPosY();
    JSON += "\"cy\":" + cy.str() + ",";
    r << a.GetRadio();
    JSON += "\"r\":" + r.str() + ",";
    angle << a.GetAngulo();
    JSON += "\"angle\":" + angle.str() + "}";
    return JSON;

}
