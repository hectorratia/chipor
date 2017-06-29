#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

vector<string> colarchivo;
vector<double> probabilidades;
int n_col=0;
bool **relaciones;


double apostados[14][3];
double porcentajes[14][3];

double premios14[5][4782969];
double probs[4782969];

int aciertos[5][4782969];
double ganado[4782969];
double emtotal;
double prentable;
double pdepremios;
double ppremios[5];

bool corteok;
double pcorte;

//Variables para i columna vs prob
vector<vector<double> > grafico;
vector<double> elem_grafico;
int ncol;

int *indices;
double recaudacion;
int columnas_deseadas;

ifstream fin;
ofstream fout;

string nombre_archivo;
string linea;

int mr_fallos;

int pot3[]={1,3,9,27,81,243,729,2187,6561,19683,59049,177147,531441,1594323};

int stoin(string& columna)
{
  int tstoin=0;
  for(int i=0 ;i<14 ; i++){
    switch(columna[i]){
      case '1':{ break;}
      case 'X':{ tstoin+=pot3[i]; break;}
      case 'x':{ tstoin+=pot3[i]; break;}
      case '2':{ tstoin+=2*pot3[i]; break;}
    }
  }

  return tstoin;
}

int stoinn(string& columna,int n)
{
  int tstoin=0;
  for(int i=0 ;i<n ; i++){
    switch(columna[i]){
      case '1':{ break;}
      case 'X':{ tstoin+=pot3[i]; break;}
      case 'x':{ tstoin+=pot3[i]; break;}
      case '2':{ tstoin+=2*pot3[i]; break;}
    }
  }

  return tstoin;
}

string intos(int index)
{
  string tintos="";
  for(int i=0; i<14 ; i++){
    switch((index / pot3[i])%3){
      case 0:{ tintos+="1"; break;}
      case 1:{ tintos+="X"; break;}
      case 2:{ tintos+="2"; break;}
    }
  }

  return tintos;
}

string intosn(int index, int n)
{
  string tintos="";
  for(int i=0; i<n ; i++){
    switch((index / pot3[i])%3){
      case 0:{ tintos+="1"; break;}
      case 1:{ tintos+="X"; break;}
      case 2:{ tintos+="2"; break;}
    }
  }

  return tintos;
}

double azar(){
  return (double)rand()/RAND_MAX;
}

int generar_col(){
  double a;
  string col;
  for(int i=0; i<14; i++){
    a=azar();
    if(a<porcentajes[i][0]){ col+='1';}
    else{ if(a<porcentajes[i][0]+porcentajes[i][1]){ col+='X';}
    else{ col+='2';}}
  }

  return stoin(col);
}

int fallos(int& index1, int& index2){
  int tfallos=0;
  int bit1;
  int bit2;

  for(int i=13; i>=0 ; i--){
    bit1=(index1/pot3[i])%3;
    bit2=(index2/pot3[i])%3;

    tfallos+= (bit1!=bit2);
  }

  return tfallos;
}

bool leer_porcentajes(){

  linea="bets.pct";
  fin.open(linea.c_str());
  if(fin.is_open()){
    for(int i=0 ;i<14 ; i++){
      fin >> porcentajes[i][0];
      fin >> porcentajes[i][1];
      fin >> porcentajes[i][2];
      porcentajes[i][0]/=100;
      porcentajes[i][1]/=100;
      porcentajes[i][2]/=100;
    }
    fin.close();
    return true;
  }else{
    return false;
  }
}

bool leer_apostados(){

  linea="lae.pct";
  fin.open(linea.c_str());
  if(fin.is_open()){
    for(int i=0 ;i<14 ; i++){
      fin >> apostados[i][0];
      fin >> apostados[i][1];
      fin >> apostados[i][2];
      apostados[i][0]/=100;
      apostados[i][1]/=100;
      apostados[i][2]/=100;
    }
    fin.close();
    return true;
  }else{
    return false;
  }
}

bool funcionordena (vector<double> i,vector<double> j) { return (i[1]>j[1]); }

void calcular_todos(){
    string columna;
    double prob;

    cout << "Calculando probabilidades." << endl;

    elem_grafico.clear();
    elem_grafico.push_back(0);    
    elem_grafico.push_back(0);    

    grafico.clear();

    for(int j=0; j<4782969; j++){
      columna=intos(j);
      prob=1;
      for(int i=0; i<14; i++){
        switch(columna[i]){
          case '1':{ prob*=porcentajes[i][0]; break;}
          case 'X':{ prob*=porcentajes[i][1]; break;}
          case '2':{ prob*=porcentajes[i][2]; break;}
        }
      }
      elem_grafico[0]=j;
      elem_grafico[1]=prob;
      grafico.push_back(elem_grafico);
    }

    cout << "Ordenando...";

    sort(grafico.begin(),grafico.end(),funcionordena);

    cout << "Ordenado." << endl;

    return;
}


void sacar_grafico(){
  double masatotal=0;

  fout.open(nombre_archivo.c_str());

  for(int i=0; i<ncol; i++){
//    cout << intos(grafico[i][0]) << " " << grafico[i][1] << endl;    
    fout << intos(grafico[i][0]) << endl;    
    masatotal+=grafico[i][1];
  }

  fout.close();
  
  cout << "Masa total: " << masatotal << endl;
  return;
}


int main(int argc, char* argv[])
{

    if(not leer_porcentajes()) return 0;
    if(not leer_apostados()) return 0;
    stringstream ss;//create a stringstream
    ss.str(argv[1]);
    ss >> ncol;
    ss.str();
    ss << ncol;//add number to the stream
    nombre_archivo=ss.str()+".cem";
//		recaudacion=2000000;
//    cout << "Columnas deseadas? ";
//    cin >> ncol;

    calcular_todos();

    sacar_grafico();

    return 0;
}
