#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <stdio.h>

using namespace std;

vector<string> colarchivo;
vector<double> probabilidades;
int n_col=0;

vector<int> elegidas;
int signos[15][3];
double porcentajes[15][3];

int minx,min2,maxx,max2;
int minv,maxv;
double mediax,media2,mediav;

bool CONPLENO=false;

ifstream fin;
ofstream fout;

string nombre_archivo;
string linea;

std::string exec(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
                result += buffer;
    }
    pclose(pipe);
    return result;
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

void contar_signos(){
  mediax=0;
  media2=0;
  minx=14;
  min2=14;
  minv=14;
  maxx=0;
  max2=0;
  maxv=0;

  int equis;
  int doses;
  int variantes;

  if(colarchivo[0][14]=='1' || colarchivo[0][14]=='X' || colarchivo[0][14]=='2') CONPLENO=true;
  for(int i=0; i<n_col; i++){
    equis=0;
    doses=0;
    for(int j=0; j<14; j++){
      switch(colarchivo[i][j]){
        case '1':{signos[j][0]++; break;}
        case 'X':{signos[j][1]++; equis++; break;}
        case '2':{signos[j][2]++; doses++; break;}
      }
    }
    if(CONPLENO){
      switch(colarchivo[i][14]){
        case '1':{signos[14][0]++; break;}
        case 'X':{signos[14][1]++; break;}
        case '2':{signos[14][2]++; break;}
      }
    }
    if(equis<minx) minx=equis;
    if(equis>maxx) maxx=equis;
    if(doses<min2) min2=doses;
    if(doses>max2) max2=doses;
    variantes=equis+doses;
    if(variantes<minv) minv=variantes;
    if(variantes>maxv) maxv=variantes;
    media2+=doses;
    mediax+=equis;
    mediav+=variantes;
  }
  return;
}

void calcular_pct(){
  for(int i=0; i<14; i++){
    porcentajes[i][0]=(double)signos[i][0]*100/n_col;
    porcentajes[i][1]=(double)signos[i][1]*100/n_col;
    porcentajes[i][2]=(double)signos[i][2]*100/n_col;
  }
  if(CONPLENO){
    porcentajes[14][0]=(double)signos[14][0]*100/n_col;
    porcentajes[14][1]=(double)signos[14][1]*100/n_col;
    porcentajes[14][2]=(double)signos[14][2]*100/n_col;
  }
  return;
}

void print_pct(){

  ifstream fequipos;
  vector<string> equipos;

  linea=exec("ls *.eqp");
  linea.resize(linea.find('.')+4,' ');
  fequipos.open(linea.c_str());
  for(int i=0; i<28; i++){
    linea="";
    getline(fequipos, linea);
    linea.resize(11,' ');
    equipos.push_back(linea);
  }
  if(CONPLENO){
    linea="";
    getline(fequipos, linea);
    linea.resize(11,' ');
    equipos.push_back(linea);
    linea="";
    getline(fequipos, linea);
    linea.resize(11,' ');
    equipos.push_back(linea);
  }
  fequipos.close();

  cout << n_col << " COLUMNAS JUGADAS" << endl;
  cout << endl;
  cout << "       P A R T I D O S          %1     %X     %2  " << endl;
  cout << "----------------------------  ------ ------ ------" << endl;
  for(int i=0; i<14; i++){
    if( i<9) cout << "0";
    cout << i+1 << ". " << equipos[i*2] << " -" << equipos[i*2+1] << ": ";
    cout << setiosflags(ios::fixed) << setprecision(2) << setw(6) << porcentajes[i][0] << " " << setiosflags(ios::fixed) << setprecision(2) << setw(6) << porcentajes[i][1] << " " << setiosflags(ios::fixed) << setprecision(2) << setw(6) << porcentajes[i][2] << endl;
  }
  if(CONPLENO){
  cout << endl;
  cout << "15. " << equipos[14*2] << " -" << equipos[14*2+1] << ": ";
  cout << setiosflags(ios::fixed) << setprecision(2) << setw(6) << porcentajes[14][0] << " " << setiosflags(ios::fixed) << setprecision(2) << setw(6) << porcentajes[14][1] << " " << setiosflags(ios::fixed) << setprecision(2) << setw(6) << porcentajes[14][2] << endl;}

  cout << endl;
  cout << "Entre " << minx << " y " << maxx << " equis." << endl;
  cout << "Entre " << min2 << " y " << max2 << " doses." << endl;
  cout << "Entre " << minv << " y " << maxv << " variantes." << endl;
  cout << "De media " << mediax/n_col << " equis, " << media2/n_col << " doses y " << mediav/n_col << " variantes." << endl;

  return;
}

int main(int argc, char* argv[])
{

    nombre_archivo=argv[1];

    leer_columnas();
    contar_signos();
    calcular_pct();
    print_pct();


    return 0;
}
