#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

vector<string> colarchivo;
vector<int> puntuacion;

string ganadora;

int n_col=0;

ifstream fin;
ofstream fout;

string nombre_archivo;
string linea;

int columnas_aciertos[15];

int aciertos(string& s1, string& s2){
  int ac=14;

  for(int i=0; i<14 ; i++){
    if(s2[i]=='-') continue;
    if(s2[i]!=s1[i]) ac--;
  }

  return ac;
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

    fin.open("ganadora.txt");
    getline(fin,ganadora);
    fin.close();

    return true;
  }else{
    return false;
  }
}

void swap(int iA, int iB){
  int pt;
  string st;

  pt=puntuacion[iA];
  st=colarchivo[iA];

  puntuacion[iA]=puntuacion[iB];
  colarchivo[iA]=colarchivo[iB];

  puntuacion[iB]=pt;
  colarchivo[iB]=st;

  return;
}

void siftDown(int start, int end){
  int root;
  int child;
  root=start;
  while(root*2+1<=end){
    child=root*2+1;
    if(child<end and puntuacion[child]<puntuacion[child+1]){
      child++;
    }
    if(puntuacion[root]<puntuacion[child]){
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

void ordenar(){
  heap_sort();

  return;
}

void calcular_aciertos(){
    int lacolumna[14];

    cout << "Calculando aciertos." << endl;

    for(int i=0; i<n_col; i++) puntuacion.push_back(aciertos(colarchivo[i],ganadora));

    cout << "Aciertos calculados." << endl;
    return;
}

void todas(){

  fout.open("ACIERTOS.txt");
  for(int i=n_col-1; i>=0; i--){
    fout << colarchivo[i] << " " << setw(2) << puntuacion[i] << endl;
  }
  fout.close();
  return;
}

void pantalla(){
  for(int i=0; i<15; i++) columnas_aciertos[i]=0;

  for(int i=0; i<n_col; i++) columnas_aciertos[puntuacion[i]]++;

  for(int i=14; i>=0; i--) cout << setw(2) << i << " " << columnas_aciertos[i] << endl;

}

int main(int argc, char* argv[])
{
    nombre_archivo=argv[1];
    leer_columnas();

    calcular_aciertos();

    ordenar();

    todas();

    pantalla();

    return 0;
}
