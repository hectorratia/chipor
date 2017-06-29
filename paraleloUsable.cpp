#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>


using namespace std;

int maxrechazos=5;
int maxintentos=40001;

vector<string> colarchivo;
vector<double> probabilidades;
int n_col=0;
bool **relaciones;

double apostados[14][3];
double porcentajes[14][3];

double premios14[5][4782969];
double probs[4782969];

int listas[1000][19321][2];
int aciertos[5][4782969];
double ganado[4782969];
double emtotal;
double prentable;
bool rentable[4782969];
double ppremios[5];
double exponente;

double recaudacion;
int columnas_deseadas;
double precio;

string archivo_semilla;
string archivo_base;
bool semilla;
bool base;

int itreset;
int itact;

vector<int> elegidas;
vector<int> relegidas;
vector<int> indices;
vector<int> rechazos;


ifstream fin;
ofstream fout;

string nombre_archivo;
string linea;

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

bool leer_exponente(){

  linea="exponente.input";
  fin.open(linea.c_str());
  if(fin.is_open()){
    fin >> exponente;
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

  a14=ceil(apuestas*p14)+1;
  a13=ceil(apuestas*p13)+1;
  a12=ceil(apuestas*p12)+1;
  a11=ceil(apuestas*p11)+1;
  a10=ceil(apuestas*p10)+1;

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

  if(premio10>=2500) premio10=premio10-(premio10-2500)*0.2;
  if(premio11>=2500) premio11=premio11-(premio11-2500)*0.2;
  if(premio12>=2500) premio12=premio12-(premio12-2500)*0.2;
  if(premio13>=2500) premio13=premio13-(premio13-2500)*0.2;
  if(premio14>=2500) premio14=premio14-(premio14-2500)*0.2;

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

void sacar_resultados(){
  cout << "Resultados de la simulacion." << endl;
  cout << endl;
  cout << "Premio   Probabilidad"<< endl;
  for(int i=0; i<5; i++){
    cout << 14-i << "           " << ppremios[i]*100 << endl;
  }
  cout << endl;
  cout << "Probabilidad de rentabilizar: " << prentable*100 << endl;
  cout << "EM total: " << emtotal << " gastando " << (double)n_col/2 << endl;
  cout << "Rentabilidad total: " << 200*emtotal/n_col << endl;

  return;
}

void meter_lista(int queindice, int quecol){
  int ilista=0;
  listas[quecol][ilista][0]=queindice;
  listas[quecol][ilista][1]=0;
	ilista++;
	
  {
    int tindex[4];
    int cambios[4];
    int tbit[4];
    //premios 13
    for(cambios[0]=0; cambios[0]<14; cambios[0]++){
      tbit[0]=(queindice/pot3[cambios[0]])%3;
      for(int j=0; j<3; j++){
        if(tbit[0]==j) continue;
        tindex[0]=queindice+(j-tbit[0])*pot3[cambios[0]];
				listas[quecol][ilista][0]=tindex[0];
				listas[quecol][ilista][1]=1;
				ilista++;				
        //premios 12
        for(cambios[1]=cambios[0]+1; cambios[1]<14; cambios[1]++){
          tbit[1]=(tindex[0]/pot3[cambios[1]])%3;
          for(int k=0; k<3; k++){
            if(tbit[1]==k) continue;
            tindex[1]=tindex[0]+(k-tbit[1])*pot3[cambios[1]];
						listas[quecol][ilista][0]=tindex[1];
						listas[quecol][ilista][1]=2;
						ilista++;
            //premios 11
            for(cambios[2]=cambios[1]+1; cambios[2]<14; cambios[2]++){
              tbit[2]=(tindex[1]/pot3[cambios[2]])%3;
              for(int l=0; l<3; l++){
                if(tbit[2]==l) continue;
                tindex[2]=tindex[1]+(l-tbit[2])*pot3[cambios[2]];
								listas[quecol][ilista][0]=tindex[2];
								listas[quecol][ilista][1]=3;
								ilista++;
                //premios 10
                for(cambios[3]=cambios[2]+1; cambios[3]<14; cambios[3]++){
                  tbit[3]=(tindex[2]/pot3[cambios[3]])%3;
                  for(int m=0; m<3; m++){
                    if(tbit[3]==m) continue;
                    tindex[3]=tindex[2]+(m-tbit[3])*pot3[cambios[3]];
										listas[quecol][ilista][0]=tindex[3];
										listas[quecol][ilista][1]=4;
										ilista++;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return;
}

void meter_columna(int quecol){
	int qindice,qpremio;
	int ii;

	#pragma omp parallel for private(ii,qindice,qpremio) reduction(+:prentable,emtotal)
	for(ii=0; ii<19321; ii++){
		qindice=listas[quecol][ii][0];
		qpremio=listas[quecol][ii][1];
		aciertos[qpremio][qindice]++;

	  ganado[qindice]+=premios14[qpremio][qindice];
		if(ganado[qindice]>precio & (not rentable[qindice])){ rentable[qindice]=true; prentable+=probs[qindice];}
		emtotal+=premios14[qpremio][qindice]*probs[qindice];		
	}

  return;
}

void sacar_columna(int quecol){
	int qindice,qpremio;
	int ii;

	#pragma omp parallel for private(ii,qindice,qpremio) reduction(-:prentable,emtotal)
	for(ii=0; ii<19321; ii++){
		qindice=listas[quecol][ii][0];
		qpremio=listas[quecol][ii][1];
		aciertos[qpremio][qindice]--;

	  ganado[qindice]-=premios14[qpremio][qindice];
		if(ganado[qindice]<precio & rentable[qindice]){ rentable[qindice]=false; prentable-=probs[qindice];}
		emtotal-=premios14[qpremio][qindice]*probs[qindice];		
	}

  return;
}

void crear_semilla(){
  for(int i=0; i<columnas_deseadas; i++){
    elegidas.push_back(i);
    relegidas.push_back(0);
  }

  for(int i=columnas_deseadas; i<4782969; i++){
    indices.push_back(i);
    rechazos.push_back(0);
  }

  return;
}

bool cargar_base(){
	int randomcol;
	
  srand ( time(NULL) );

  fin.open(archivo_base.c_str());
  if(fin.is_open()){
    while(getline(fin,linea)){
      indices.push_back(stoin(linea));
      rechazos.push_back(0);
    }
    
    for(int i=0; i<columnas_deseadas; i++){
    	randomcol=rand() % indices.size();
      elegidas.push_back(indices[randomcol]);      
      relegidas.push_back(0);
      indices.erase(indices.begin()+randomcol);
    }

    fin.close();
    return true;
  }else{
    return false;
  }
}

bool cargar_semilla(){

  bool estan[4782969];

  fin.open(archivo_semilla.c_str());
  if(fin.is_open()){
    while(getline(fin,linea)){
      elegidas.push_back(stoin(linea));
      relegidas.push_back(0);
    }
    columnas_deseadas=elegidas.size();
    fin.close();

    for(int i=0; i<4782969; i++){
      estan[i]=false;
    }

    for(int i=0; i<columnas_deseadas; i++){
      estan[elegidas[i]]=true;
    }

    for(int i=0; i<4782969; i++){
      if(not estan[i]){
        indices.push_back(i);
        rechazos.push_back(0);
      }
    }

    return true;
  }else{
    return false;
  }
}

bool cargar_semillaybase(){

  bool estan[4782969];

  fin.open(archivo_semilla.c_str());
  if(fin.is_open()){
    while(getline(fin,linea)){
      elegidas.push_back(stoin(linea));
      relegidas.push_back(0);
    }
    columnas_deseadas=elegidas.size();
    fin.close();

    for(int i=0; i<4782969; i++){
      estan[i]=false;
    }

    for(int i=0; i<columnas_deseadas; i++){
      estan[elegidas[i]]=true;
    }

    fin.close();
    fin.open(archivo_base.c_str());
    while(getline(fin,linea)){
      if(not estan[stoin(linea)]){
      indices.push_back(stoin(linea));
      rechazos.push_back(0);
      }
    }
    return true;
  }else{
    return false;
  }
}


void reset_escrutinio(){
  emtotal=0;
  prentable=0;

  for(int i=0; i<4782969; i++){
    aciertos[0][i]=0;
    aciertos[1][i]=0;
    aciertos[2][i]=0;
    aciertos[3][i]=0;
    aciertos[4][i]=0;
    ganado[i]=0;
    rentable[i]=false;
  }

  for(int i=0; i<columnas_deseadas; i++){
  	meter_lista(elegidas[i],i);
    meter_columna(i);
  }

  return;
}

void escribir_fichero(){
  fout.open(nombre_archivo.c_str());
  for(int i=0; i<columnas_deseadas; i++) fout << intos(elegidas[i]) << endl;
  fout.close();

  return;
}


int main()
{
    if(not leer_porcentajes()) return 0;
    if(not leer_apostados()) return 0;

    char c;
    cout << "STOCHASTIC TUNNELING RAIZ" << endl;

    if(not leer_exponente()) return 0;
    cout << "Exponente: " << exponente << endl;

    cout << "Quieres usar una semilla? ";
    cin >> c;
    if(c=='s'){
      semilla=true;
      cout << "Archivo semilla? ";
      cin >> archivo_semilla;
    }else{
      semilla=false;
    }

    cout << "Quieres usar una base? ";
    cin >> c;
    if(c=='s'){
      base=true;
      cout << "Archivo base? ";
      cin >> archivo_base;
    }else{
      base=false;
    }
		
    if(not semilla){
      cout << "Cuantas columnas? ";
      cin >> columnas_deseadas;
    }

/*    cout << "Reseteo cada cuantas iteraciones? ";
    cin >> itreset;
    cout << "Actualizar cada cuantas iteraciones? ";
    cin >> itact;*/
    cout << "Recaudacion esperada en la jornada? ";
    cin >> recaudacion;
    itreset=10000;
    itact=1000;
    
    cout << "A que archivo quieres grabar? ";
    cin >> nombre_archivo;


    //Calcular premios
    calcular_todos();

    //Elegir una semilla
    if(semilla){
      if(base){
        if(not cargar_semillaybase()) return 0;
        cout << "Semilla y base cargadas." << endl;
      }else{
        if(not cargar_semilla()) return 0;
        cout << "Semilla cargada." << endl;
      }
    }else{
      if(base){
        if(not cargar_base()) return 0;
        cout << "Base cargada." << endl;
      }else{
        crear_semilla();
        cout << "Semilla creada." << endl;
      }
    }

    precio=columnas_deseadas/2;


    //Escrutinio de la semilla
    reset_escrutinio();
    cout << "Escrutinio de la semilla terminado." << endl;

    double valoracion;



    

    valoracion=pow(emtotal/precio,exponente)*prentable;
    cout << "Valoracion inicial: "<< valoracion << ". Rentabilidad: " << 100*emtotal/precio << ". Prentable: " << prentable*100 <<"." << endl;

    //Ciclo por todas las columnas
    /*
      1-Elegir una columna que no este en la semilla
      2-Intercambiar con una de la semilla:
          -Restar el escrutinio de la que sale
          -Anadir el escrutinio de la que entra
      3-Si rentabilidad*prentabilizar > que antes conservar el cambio, si no
        deshacer el cambio y anadir un punto a la columna que entraba.
      4-Si la columna acumula mas de 5, 10, 15 o los cambios que sean, borrarla

      5-Sacar por pantalla el maximo conseguido.
    */
    int entrar;
    int salir;
    double nueva_valoracion;

    double f0;
    double fstun1;
    double fstun2;
    double media;
    double azar;

    double gamma;
    double beta;
    double aceptar;

    f0=-valoracion;
    fstun1=0;
    media=0;

    gamma=1000;
    beta=1000;

    int ti;
    int tr;

    int i=1;
    srand ( time(NULL) );

    while(true){
      entrar=( rand() % indices.size() );
      salir=( rand() % columnas_deseadas );

      sacar_columna(salir);
      meter_lista(indices[entrar],salir);
      meter_columna(salir);

      nueva_valoracion=pow(emtotal/precio,exponente)*prentable;

      if((-nueva_valoracion)<f0) f0=-nueva_valoracion;

      fstun2=1-exp(-gamma*(-nueva_valoracion-f0));

      media=(media*4+fstun1)/5;
      if(media>=0.03){
        beta-=0.1;
      }else{
        beta+=0.1;
      }

      aceptar=exp(-beta*(fstun2-fstun1));

      //ACEPTACION
      azar=(double)rand()/RAND_MAX;
//      cout << azar << " " << aceptar << endl;
//      cin >> c;
      if(azar<aceptar){
        ti=elegidas[salir];
        tr=relegidas[salir];

        elegidas[salir]=indices[entrar];
        relegidas[salir]=rechazos[entrar];

        indices[entrar]=ti;
        rechazos[entrar]=tr;

        valoracion=nueva_valoracion;
        fstun1=fstun2;

      }else{
        //NO ACEPTACION
        rechazos[entrar]++;
        sacar_columna(salir);
        meter_lista(elegidas[salir],salir);
        meter_columna(salir);

        if(rechazos[entrar]>maxrechazos){
          indices.erase(indices.begin()+entrar);
          rechazos.erase(rechazos.begin()+entrar);
          if(indices.size()==0) break;
        }
      }

      if((i % itreset)==0){
        reset_escrutinio();
        escribir_fichero();
      }
      if((i % itact)==0){
        valoracion=pow(emtotal/precio,exponente)*prentable;
        cout << "It "<< i << ". Valoracion: " << valoracion << ". R: " << 100*emtotal/precio << ". Pren: " << prentable*100 <<". Quedan: "<< indices.size() << "." << endl;
      }
      i++;
//      if(i>300000) break;
    }

    cout << "Terminado en " << i << " iteraciones." << endl;
    cout << "Valoracion: " << valoracion << ". Rentabilidad: " << 100*emtotal/precio << ". Prentable: " << prentable*100 <<"." << endl;
    escribir_fichero();

    return 0;
}
