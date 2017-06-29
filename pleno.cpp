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
double recaudacion, bote;
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

bool leer_apostados(){

  linea="lae.pct";
  fin.open(linea.c_str());
  if(fin.is_open()){
    for(int i=0 ;i<15 ; i++){
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

bool leer_columnas(){

  linea=nombre_archivo;
  fin.open(linea.c_str());
  if(fin.is_open()){
    while(getline(fin,linea)){
      colarchivo.push_back(linea);
      n_col++;
    }
    fin.close();
    return true;
  }else{
    return false;
  }
}

double pbet_col(int que_col){
  double prob=1.0;

  for(int i=0; i<14; i++){
    switch(colarchivo[que_col][i]){
      case '1':{ prob*=porcentajes[i][0]; break;}
      case 'X':{ prob*=porcentajes[i][1]; break;}
      case '2':{ prob*=porcentajes[i][2]; break;}
    }
  }
  return prob;
}

double plae_col(int que_col){
  double prob=1.0;

  for(int i=0; i<14; i++){
    switch(colarchivo[que_col][i]){
      case '1':{ prob*=apostados[i][0]; break;}
      case 'X':{ prob*=apostados[i][1]; break;}
      case '2':{ prob*=apostados[i][2]; break;}
    }
  }
  return prob;
}

void calcular_probs(){

  for(int i=0; i<n_col; i++){
    pbet.push_back(pbet_col(i));
    plae.push_back(plae_col(i));
  }

  return;
}

void crear_indices(){
  indices=new int[n_col];

  for(int i=0; i<n_col; i++){
    indices[i]=stoin(colarchivo[i]);
  }
  return;
}

int main(int argc, char* argv[])
{
    if(not leer_porcentajes()) return 0;
    if(not leer_apostados()) return 0;

    nombre_archivo=argv[1];
    cout << "Recaudacion esperada en la jornada? ";
    cin >> recaudacion;

    cout << "Bote esperado en la jornada? ";
    cin >> bote;

    leer_columnas();

    calcular_probs();

    crear_indices();

    cout << "Archivo de salida? ";
    cin >> nombre_archivo;

    fout.open(nombre_archivo.c_str());

    int cuentasignos[3];
    cuentasignos[0]=0;
    cuentasignos[1]=0;
    cuentasignos[2]=0;

    for(int i=0; i<n_col; i++){
        double acertantes15[3];
        double premio15[3];
        double em15[3];
        double maxem15=-1;
        int eleccion=-1;
        for(int j=0; j<3; j++){
            acertantes15[j]=ceil(plae[i]*apostados[14][j]*recaudacion*2);
            premio15[j]=(recaudacion*0.1+bote)/acertantes15[j];
            if(premio15[j]>2500) premio15[j]=premio15[j]-(premio15[j]-2500)*0.2;
            //cout << premio15[j] << " ";
            em15[j]=premio15[j]*pbet[i]*porcentajes[14][j];
//            cout << em15[j] << " ";
            if(em15[j]>maxem15){ maxem15=em15[j]; eleccion=j;}
        }
//        cout << maxem15 << " " << eleccion << endl;
        if(eleccion==0) fout << intos(indices[i])+"1" << endl;
        if(eleccion==1) fout << intos(indices[i])+"X" << endl;
        if(eleccion==2) fout << intos(indices[i])+"2" << endl;
//	cout << premio15[eleccion] << " " << eleccion << " " << maxem15 << endl;
        cuentasignos[eleccion]++;
    }
    fout.close();
    cout << cuentasignos[0]+cuentasignos[1]+cuentasignos[2] << " " << n_col << endl;
    cout << (double)100*cuentasignos[0]/n_col << " " << (double)100*cuentasignos[1]/n_col << " " << (double)100*cuentasignos[2]/n_col << endl;

    return 0;
}
