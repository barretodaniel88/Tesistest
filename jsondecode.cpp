#include "jsondecode_h.h"

using namespace std;

/*
string getBodyByPos(string, int);
Cuerpo JSON2Cuerpo(string);
Agente JSON2Agente(string);
void getAllObjects(string, Cuerpo *, Agente *);
string buildJSONpackage(Cuerpo *, Agente *);
string cuerpo2JSON(Cuerpo);
string agente2JSON(Agente);
string JSONgetString(string, string);
float JSONgetNum(string, string);
*/

//string jsonstring;
//char buffer[1000] = "{\"config\":{\"tipo\":\"inicio\",\"long\":3},\"objetos\":[{\"cx\":1.34,\"cy\":2.2,\"r\":10},{\"cx\":3,\"cy\":-6.3,\"r\":20,\"angle\":3.4325},{\"cx\":3,\"cy\":4,\"r\":30}]}";
//size_t found, begin, end;

//Cuerpo misobstaculos[20];
//Agente misagentes[20];
//int nagentes = 0, nobstaculos = 0;

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

Cuerpo JSON2Cuerpo(string c)
{
    float cx, cy, r;

    cx = JSONgetNum(c, "cx");
    cy = JSONgetNum(c, "cy");
    r = JSONgetNum(c, "r");

    Cuerpo mycuerpo(cx,cy,r);
    cout << "El cuerpo tiene: \n" << "\tcx: " << cx << "\n\tcy: " << cy << "\n\tr: " << r << "\n";
    return mycuerpo;
}

Agente JSON2Agente(string c)
{
    float cx, cy, r, ang;

    cx = JSONgetNum(c, "cx");
    cy = JSONgetNum(c, "cy");
    r = JSONgetNum(c, "r");
    ang = JSONgetNum(c, "angle");

    Agente myagente(cx,cy,r,1,1,1);
    myagente.SetAngulo(ang);
    cout << "El cuerpo tiene: \n" << "\tcx: " << cx << "\n\tcy: " << cy << "\n\tr: " << r << "\n\tangle: " << ang << "\n";
    return myagente;
}

void getAllObjects(string jsonstring, Cuerpo *c, Agente *a)
{
    //int nagentes = 0, nobstaculos = 0;
    int length = (int)JSONgetNum(jsonstring, "long");
    cout << jsonstring;
    cout << "El paquete tiene " << length << " obstaculos\n";
    string temp;
    for(int i=0; i<length; i++)
    {
        temp = getBodyByPos(jsonstring,i);
        if(temp.find("angle") == string::npos)
        {
            cout << "El cuerpo de la posicion " << i << " es obstaculo\n";
            c[ncuerpos] = JSON2Cuerpo(temp);
            ncuerpos++;

        }else{
            cout << "El cuerpo de la posicion " << i << " es agente\n";
            a[nagentes] = JSON2Agente(temp);
            nagentes++;
        }
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

char * buildJSONpackage(Cuerpo *c, Agente *a)
{
    string jsonstring = "{\"config\":{\"tipo\":\"final\",\"long\":";
    stringstream temp;
    temp << ncuerpos+nagentes;
    jsonstring += temp.str();
    jsonstring += "},\"objects\":[";
    cout << "ncuerpos: " << ncuerpos;
    for(int i=0; i< ncuerpos; i++){
        cout << "entre";
        jsonstring += cuerpo2JSON(c[i]) + ",";
    }
    for(int i=0; i< nagentes; i++){
        jsonstring += agente2JSON(a[i]);
        if(i<nagentes-1)    jsonstring += ",";
    }
    jsonstring += "]}";
    cout << jsonstring << "\n";
    return (char *)jsonstring.c_str();
    //return jsonstring;
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

