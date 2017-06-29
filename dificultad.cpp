#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

vector<string> colarchivo;
vector<double> probabilidades;
int n_col=0;
bool **relaciones;

double apostados[14][3];
double porcentajes[14][3];

double premios14[5][4782969];
double *em;
double probs[4782969];

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

  linea="bets.pct";
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

double probabilidad_col(int que_col){
  double prob=1.0;

  for(int i=0; i<14; i++){
    switch(colarchivo[que_col][i]){
      case '1':{ prob*=porcentajes[i][0]/apostados[i][0]; break;}
      case 'X':{ prob*=porcentajes[i][1]/apostados[i][1]; break;}
      case '2':{ prob*=porcentajes[i][2]/apostados[i][2]; break;}
    }
  }
  return prob;
}

void calcular_probs(){

  for(int i=0; i<n_col; i++){
    probabilidades.push_back(probabilidad_col(i));
  }

  return;
}

void swap(int iA, int iB){
  int it;
  string st;
  double dt;

  it=indices[iA];
  st=colarchivo[iA];
  dt=em[iA];

  indices[iA]=indices[iB];
  colarchivo[iA]=colarchivo[iB];
  em[iA]=em[iB];

  indices[iB]=it;
  colarchivo[iB]=st;
  em[iB]=dt;

  return;
}

void siftDown(int start, int end){
  int root;
  int child;
  root=start;
  while(root*2+1<=end){
    child=root*2+1;
    if(child<end and em[child]<em[child+1]){
      child++;
    }
    if(em[root]<em[child]){
      swap(root,child);
      root=child;
    }else{
      return;
    }
  }
  return;
}

void heapify(){
  int start;
  start=(n_col-2)/2;
  while(start>=0){
    siftDown(start,n_col-1);
    start--;
  }
  return;
}

void heap_sort(){

  heapify();

  int end;

  end=n_col-1;

  while(end>0){
    swap(end,0);
    siftDown(0,end-1);
    end--;
  }

  return;
}

void crear_indices(){
  indices=new int[n_col];
  em=new double[n_col];

  for(int i=0; i<n_col; i++){
    indices[i]=stoin(colarchivo[i]);
    em[i]=0;
  }
  return;
}

void ordenar_em(){
  heap_sort();

  return;
}

void calcular_premios(int columna[]){

  double p14=1.0;
  double p13=1.0;
  double p12=1.0;
  double p11=1.0;
  double p10=1.0;

  double cr[14];
  double suma[14];

  double suma13=0;
  double suma12=0;
  double suma11=0;
  double suma10=0;

  double premio14;
  double premio13;
  double premio12;
  double premio11;
  double premio10;

  int a14; //Acertantes de 14
  int a13;
  int a12;
  int a11;
  int a10;
  double apuestas;

  double prob=1;

  for(int i=0; i<14; i++){
    switch(columna[i]){
      case 0:{ prob*=porcentajes[i][0]; p14*=apostados[i][0]; cr[i]=(1-apostados[i][0])/apostados[i][0]; break;}
      case 1:{ prob*=porcentajes[i][1]; p14*=apostados[i][1]; cr[i]=(1-apostados[i][1])/apostados[i][1]; break;}
      case 2:{ prob*=porcentajes[i][2]; p14*=apostados[i][2]; cr[i]=(1-apostados[i][2])/apostados[i][2]; break;}
    }
    suma13+=cr[i];
  }
  p13=p14*suma13;

  suma[0]=suma13;
  for(int i=1; i<14; i++){
    suma[i]=suma[i-1]-cr[i-1];
    suma12+=suma[i]*cr[i-1];
  }
  p12=p14*suma12;

  suma[0]=suma12;
  for(int i=1; i<13; i++){
    suma[i]=suma[i-1]-cr[i-1]*suma[i];
    suma11+=suma[i]*cr[i-1];
  }
  p11=p14*suma11;

  suma[0]=suma11;
  for(int i=1; i<12; i++){
    suma[i]=suma[i-1]-cr[i-1]*suma[i];
    suma10+=suma[i]*cr[i-1];
  }
  p10=p14*suma10;

  apuestas=recaudacion*2;

  a14=ceil(apuestas*p14);
  a13=ceil(apuestas*p13);
  a12=ceil(apuestas*p12);
  a11=ceil(apuestas*p11);
  a10=ceil(apuestas*p10);

  premio14=recaudacion*0.12/a14;
  premio13=recaudacion*0.08/a13;
  premio12=recaudacion*0.08/a12;
  premio11=recaudacion*0.08/a11;
  premio10=recaudacion*0.09/a10;

/*  premio14=0.06/p14;
  premio13=0.04/p13;
  premio12=0.04/p12;
  premio11=0.04/p11;
  premio10=0.045/p10;*/

//  if(premio14>recaudacion*0.12) premio14=recaudacion*0.12;
//  if(premio13>recaudacion*0.08) premio13=recaudacion*0.08;
  if(premio10<1){ premio10=0;}
  if(premio11<1){ premio11=0; premio12=recaudacion*0.16/a12;}

  int index=0;
  for(int i=13; i>0; i--){
    index+=columna[i];
    index*=3;
  }
  index+=columna[0];

  premios14[0][index]=premio14;
  premios14[1][index]=premio13;
  premios14[2][index]=premio12;
  premios14[3][index]=premio11;
  premios14[4][index]=premio10;

  probs[index]=prob;

  return;

}

void calcular_todos(){
    int lacolumna[14];

    cout << "Calculando premios." << endl;
    for(int i=0; i<14; i++) lacolumna[i]=0;

    while(1){
      calcular_premios(lacolumna);
      lacolumna[0]++;
      for(int i=0; i<13; i++){
        if(lacolumna[i]==3){ lacolumna[i+1]++; lacolumna[i]=0;}
      }
      if(lacolumna[13]==3) break;
    }

    cout << "Calculados todos los premios." << endl;
    return;
}

void calcular_em(){
  int indice;
  double maxem=0;

  for(int i=0; i<n_col; i++){
    em[i]=premios14[0][indices[i]];
    //calculo em otros premios
  }

  return;

}

void todas(){

  fout.open("DIFICULTAD.txt");
  for(int i=n_col-1; i>=0; i--){
    fout << colarchivo[i] << " " << setw(8) << (int)em[i] << endl;
  }
  fout.close();
  return;
}

int main(int argc, char* argv[])
{
    if(not leer_porcentajes()) return 0;
    if(not leer_apostados()) return 0;

    nombre_archivo=argv[1];
    cout << "Recaudacion esperada en la jornada? ";
    cin >> recaudacion;

    leer_columnas();

    calcular_todos();

    crear_indices();

    calcular_em();

    ordenar_em();

    todas();

    return 0;
}
