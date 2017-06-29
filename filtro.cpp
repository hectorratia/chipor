#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

vector<string> colarchivo;
vector<double> probabilidades;
int n_col=0;
bool **relaciones;

double apostados[15][3];
double porcentajes[15][3];

vector<double> pbet;
vector<double> plae;


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

bool leer_porcentajes(){

  linea="bets.pct";
  fin.open(linea.c_str());
  if(fin.is_open()){
    for(int i=0 ;i<15 ; i++){
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

double pbet_col(string que_col){
  double prob=1.0;

  for(int i=0; i<14; i++){
    switch(que_col[i]){
      case '1':{ prob*=porcentajes[i][0]; break;}
      case 'X':{ prob*=porcentajes[i][1]; break;}
      case '2':{ prob*=porcentajes[i][2]; break;}
    }
  }
  return prob;
}


int main(int argc, char* argv[])
{
    if(not leer_porcentajes()) return 0;

    nombre_archivo=argv[1];
    cout << "Recaudacion esperada en la jornada? ";
    cin >> recaudacion;

		double limitepremio;
    cout << "Premio maximo? ";
    cin >> limitepremio;

    fout.open(nombre_archivo.c_str());
		
		n_col=0;
    for(int i=0; i<4782969; i++){
			if((0.06/pbet_col(intos(i)))<limitepremio){
				n_col++;
				fout << intos(i) << endl;
			}			
    }
    fout.close();
		
		cout << n_col << " columnas pasan el filtro." << endl;
    return 0;
}
