#ifndef JSONDECODE_H
#define JSONDECODE_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>

#include "agente_h.h"


extern int ncuerpos;
extern int nagentes;

string getBodyByPos(string, int);
Cuerpo JSON2Cuerpo(string);
Agente JSON2Agente(string);
void getAllObjects(string, Cuerpo *, Agente *);
char *buildJSONpackage(Cuerpo *, Agente *);
string cuerpo2JSON(Cuerpo);
string agente2JSON(Agente);
string JSONgetString(string, string);
float JSONgetNum(string, string);

#endif // JSONDECODE_H
