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
double ppremiosm[5];
double ppremiosmc[5];

bool corteok;
double pcorte;

//Para el grafico de simulacion tren
int simtren[1001];
int simulaciones=10000;
bool CORTATREN=false;
bool FULLPREMIOS=false;

//Variables para grafico premio vs prob
vector<vector<double> > grafico;
vector<double> elem_grafico;

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

bool leer_recaudacion(){

  linea="recaudacion.txt";
  fin.open(linea.c_str());
  if(fin.is_open()){
    fin >> recaudacion;
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

void crear_indices(){
  indices=new int[n_col];

  for(int i=0; i<n_col; i++){
    indices[i]=stoin(colarchivo[i]);
  }
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
	int ganadora[14];

//    cout << "Calculando premios." << endl;
    fin.open("ganadora.txt");
    getline(fin,linea);
    fin.close();
    for(int i=0; i<14; i++){
    	if(linea[i]=='1'){ porcentajes[i][0]=1; porcentajes[i][1]=0; porcentajes[i][2]=0;}
    	if(linea[i]=='2'){ porcentajes[i][0]=0; porcentajes[i][1]=0; porcentajes[i][2]=1;}
    	if(linea[i]=='X'){ porcentajes[i][0]=0; porcentajes[i][1]=1; porcentajes[i][2]=0;}
    }
    
    for(int i=0; i<14; i++) lacolumna[i]=0;

    while(1){
      calcular_premios(lacolumna);
      lacolumna[0]++;
      for(int i=0; i<13; i++){
        if(lacolumna[i]==3){ lacolumna[i+1]++; lacolumna[i]=0;}
      }
      if(lacolumna[13]==3) break;
    }

//    cout << "Calculados todos los premios." << endl;
    return;
}

void calcular_escrut(){

//  cout << "Simulando." << endl;

  //contar premios
  for(int i=0; i<n_col; i++){
    aciertos[0][indices[i]]++;
    {
      int tindex[4];
      int cambios[4];
      int tbit[4];
      //premios 13
      for(cambios[0]=0; cambios[0]<14; cambios[0]++){
        tbit[0]=(indices[i]/pot3[cambios[0]])%3;
        for(int j=0; j<3; j++){
          if(tbit[0]==j) continue;
          tindex[0]=indices[i]+(j-tbit[0])*pot3[cambios[0]];
          aciertos[1][tindex[0]]++;
          //premios 12
          for(cambios[1]=cambios[0]+1; cambios[1]<14; cambios[1]++){
            tbit[1]=(tindex[0]/pot3[cambios[1]])%3;
            for(int k=0; k<3; k++){
              if(tbit[1]==k) continue;
              tindex[1]=tindex[0]+(k-tbit[1])*pot3[cambios[1]];
              aciertos[2][tindex[1]]++;
              //premios 11
              for(cambios[2]=cambios[1]+1; cambios[2]<14; cambios[2]++){
                tbit[2]=(tindex[1]/pot3[cambios[2]])%3;
                for(int l=0; l<3; l++){
                  if(tbit[2]==l) continue;
                  tindex[2]=tindex[1]+(l-tbit[2])*pot3[cambios[2]];
                  aciertos[3][tindex[2]]++;
                  //premios 10
                  for(cambios[3]=cambios[2]+1; cambios[3]<14; cambios[3]++){
                    tbit[3]=(tindex[2]/pot3[cambios[3]])%3;
                    for(int m=0; m<3; m++){
                      if(tbit[3]==m) continue;
                      tindex[3]=tindex[2]+(m-tbit[3])*pot3[cambios[3]];
                      aciertos[4][tindex[3]]++;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    // Calcular premios, rentabilidad, porcentajes


//    if( i % 100000 == 0) cout << "Pasadas " << i << " columnas." << endl;
  }
  emtotal=0;
  pdepremios=0;
  for(int i=0; i<4782969; i++){
    ganado[i]=0;
    bool maxp=true;
    for(int j=0; j<5; j++){
      ganado[i]+=premios14[j][i]*aciertos[j][i];
      if(aciertos[j][i]>0) ppremios[j]+=probs[i];
      if(aciertos[j][i]>0 && maxp && premios14[j][i]>0){ ppremiosmc[j]+=probs[i];}
      if(aciertos[j][i]>0 && maxp){ ppremiosm[j]+=probs[i]; maxp=false;}
    }
    if(ganado[i]>(n_col/2)) prentable+=probs[i];
    if(ganado[i]>0){
      pdepremios+=probs[i];
      elem_grafico.clear();
      elem_grafico.push_back(ganado[i]);
      elem_grafico.push_back(probs[i]);
      grafico.push_back(elem_grafico);
    }
    emtotal+=ganado[i]*probs[i];
  }
//  cout << "Simulacion terminada." << endl;

  return;

}

void sacar_resultados(){
  cout << "Resultados de la simulacion." << endl;
  cout << endl;
  cout << "Premio   Probabilidad"<< endl;
  for(int i=0; i<5; i++){
    if(FULLPREMIOS){
    cout << 14-i << "           " << ppremios[i]*100 << "           " << ppremiosm[i]*100 << "           " << ppremiosmc[i]*100 << endl;
    }else{
    cout << 14-i << "           " << ppremios[i]*100 << endl;
    }
  }
  if(FULLPREMIOS){ cout << "Sin cobrar: " << (1-ppremiosmc[0]-ppremiosmc[1]-ppremiosmc[2]-ppremiosmc[3]-ppremiosmc[4])*100 << endl;}
  cout << endl;
  cout << "Probabilidad de rentabilizar: " << prentable*100 << endl;
  cout << "Probabilidad de premios: " << pdepremios*100 << endl;
  if(corteok)  cout << "Probabilidad de pasar el corte: " << pcorte*100 << endl;
  cout << "EM total: " << emtotal << " gastando " << (double)n_col/2 << endl;
  cout << "Rentabilidad total: " << 200*emtotal/n_col << endl;

  return;
}


void diferencias2(int maxfallos){
  vector<int> elegidas;
  bool entra;

  elegidas.push_back(indices[n_col-1]);
  for(int i=n_col-2; i>=0; i--){
    entra=true;
    for(int j=0; j<elegidas.size(); j++){
      if(fallos(elegidas[j],indices[i])<=maxfallos){
        entra=false;
        break;
      }
    }
    if(entra) elegidas.push_back(indices[i]);
  }


  switch(maxfallos){
    case 1: linea=nombre_archivo+".c13"; break;
    case 2: linea=nombre_archivo+".c12"; break;
    case 3: linea=nombre_archivo+".c11"; break;
    case 4: linea=nombre_archivo+".c10"; break;
    case 5: linea=nombre_archivo+".c09"; break;
  }

  fout.open(linea.c_str());
  for(int i=0; i<elegidas.size(); i++) fout << intos(elegidas[i]) << endl;
  fout.close();

  cout << "Al " << 14-maxfallos << " se jugarian " << elegidas.size() << "/"<< n_col << "("<< (int)100*elegidas.size()/n_col << "%) por " << (double)elegidas.size()/2 << " euros." << endl;


  return;

}

bool funcionordena (vector<double> i,vector<double> j) { return (i[0]>j[0]); }

void sacar_tren(){

  fout.open("tren.txt");
  double totalsimtren=0;
  for(int i=0; i<=1000; i++){
    totalsimtren+=(double)simtren[i]/simulaciones;
    fout << i << " " << (double)simtren[i]/simulaciones << " " << totalsimtren << endl;
  }

  fout.close();

  return;
}

void sacar_grafico(){
  sort(grafico.begin(),grafico.end(),funcionordena);

  fout.open("grafico.txt");
  double total=0;
  for(int i=0; i<grafico.size(); i++){
    if(grafico[i][1]>0.00000001)
    fout << total << " " << grafico[i][0] << endl;
    total+=grafico[i][1];
  }
  fout << total << " 0" << endl;

  fout.close();

  return;
}

bool funcionordena2 (double i,double j) { return (i>j); }

void sacar_grafico2(){

  cout << "Simulacion temporada(50jor). Cuantos ciclos aleatorios?" << endl;

  int n_temp;
  double p_temp;
  vector<double> rentabilidades;

  cin >> n_temp;
  p_temp=1/(double)n_temp;

  double gasto;
  gasto=50*n_col/2;

  for(int i=0; i<n_temp; i++){
    double premio=0;

    for(int j=0; j<50; j++){
      int col;
      col=generar_col();
      premio+=ganado[col];
    }
    rentabilidades.push_back(premio/gasto*100);
  }
  cout << "Simulacion terminada." << endl;

  sort(rentabilidades.begin(),rentabilidades.end(),funcionordena2);

  fout.open("grafico1T.txt");
  double total=0;
  fout << total << " " << rentabilidades[0] << endl;
  for(int i=0; i<rentabilidades.size(); i++){
    total+=p_temp;
    fout << total << " " << rentabilidades[i] << endl;
  }
  fout.close();
  return;
}

void sacar_grafico3(){

  cout << "Simulacion 10 temporada(500jor). Cuantos ciclos aleatorios?" << endl;

  int n_temp;
  double p_temp;
  vector<double> rentabilidades;

  cin >> n_temp;
  p_temp=1/(double)n_temp;

  double gasto;
  gasto=500*n_col/2;

  for(int i=0; i<n_temp; i++){
    double premio=0;

    for(int j=0; j<500; j++){
      int col;
      col=generar_col();
      premio+=ganado[col];
    }
    rentabilidades.push_back(premio/gasto*100);
  }
  cout << "Simulacion terminada." << endl;

  sort(rentabilidades.begin(),rentabilidades.end(),funcionordena2);

  fout.open("grafico10T.txt");
  double total=0;
  fout << total << " " << rentabilidades[0] << endl;
  for(int i=0; i<rentabilidades.size(); i++){
    total+=p_temp;
    fout << total << " " << rentabilidades[i] << endl;
  }
  fout.close();
  return;
}

void simulacion_tren(){

  int isimtren;
  double jornadas=0;
  double valoracion;
  double oldvaloracion;
  double iprecision;
  double sigma;
  double X2;
  double X;
  double EX2;
  double EX;
  vector<double> Xi;
  int rent10=0;
  int rent50=0;
//  cout << "Empiezo simulacion tren." << endl;
  srand ( time(NULL) );

  for(int i=0; i<simulaciones; i++){
    double gasto=0;
    double ganancia=0;
    isimtren=0;
    do{
      int col=generar_col();
      gasto+=n_col/2;
      ganancia+=ganado[col];
      jornadas++;
      isimtren++;
      if(isimtren==1000){ break;}
//      if(CORTATREN && isimtren==1000){ break;}
//      cout << gasto << " " << ganancia << endl;
    }while(gasto>ganancia);
    if(isimtren<=10) rent10++;
    if(isimtren<=50) rent50++;
    if(isimtren<=1000){ simtren[isimtren]++;}else{ simtren[1000]++;}
//    cout << jornadas << endl;
    
  }

  jornadas=jornadas/simulaciones;

/*  iprecision=0;
  X=0;
  X2=0;
  do{
    X=0;
    for(int i=0; i<10000; i++){
      double gasto=0;
      double ganancia=0;
      isimtren=0;
      do{
        int col=generar_col();
        gasto+=n_col/2;
        ganancia+=ganado[col];
        jornadas++;
        isimtren++;
        if(CORTATREN && isimtren==1000){ break;}
  //      cout << gasto << " " << ganancia << endl;
      }while(gasto>ganancia);
      if(isimtren<=10) rent10++;
      if(isimtren<=50){ rent50++; X++;}
      if(isimtren<=1000){ simtren[isimtren]++;}else{ simtren[1000]++;}
//      X+=isimtren;
  //    cout << jornadas << endl;
    }
    iprecision++;
//    oldvaloracion=valoracion;
//    valoracion=jornadas/(iprecision*1000);
//  }while(iprecision<5 || abs(valoracion-oldvaloracion)>0.00001);
    X=X/10000;
    X2=(X2*(iprecision-1)+X)/(iprecision);
    Xi.push_back(X);
//    EX2=X2/pow(iprecision*1000);
//    sigma=sqrt(abs(EX2-pow(EX,2)));
    if(iprecision>10){
    sigma=0;
    for(int i=0; i<Xi.size(); i++){
      sigma+=pow(Xi[i]-X2,2);
    }
    sigma=sqrt(sigma/(Xi.size()-1));
    cout << 2*sigma << " " << X << endl;}else{sigma=1;}
    
  }while(2*sigma>0.001);
//  cout << X << " " << X2 << endl;
  simulaciones=iprecision*10000;*/

//  cout << iprecision << endl;
  cout << "Rent10: " << (double)rent10/simulaciones << " Rent50: " << (double)rent50/simulaciones << " Media: " << jornadas << endl;
//  if(CORTATREN){ cout << simtren[1000] << " ";}
  cout << simtren[1000] << " ";

//  cout << "Necesarias de media " << jornadas << " para llegar a rentabilidad." << endl;

  return;
}

void especial_tren(){

  int isimtren;
  double jornadas=0;
  double valoracion;
  double oldvaloracion;
  double iprecision;
  double sigma;
  double X2;
  double X;
  double EX2;
  double EX;
  vector<double> Xi;
  int rent10=0;
  int rent50=0;
  int rent100=0;
//  cout << "Empiezo simulacion tren." << endl;
  srand ( time(NULL) );

  simulaciones=100000;
  for(int i=0; i<simulaciones; i++){
    double gasto=0;
    double ganancia=0;
    isimtren=0;
    do{
      int col=generar_col();
      gasto+=n_col/2;
      ganancia+=ganado[col];
      jornadas++;
      isimtren++;
      if(isimtren==101){ break;}
//      if(CORTATREN && isimtren==1000){ break;}
//      cout << gasto << " " << ganancia << endl;
    }while(gasto>ganancia);
    if(isimtren<=10) rent10++;
    if(isimtren<=50) rent50++;
    if(isimtren<=100) rent100++;
//    cout << jornadas << endl;
    
  }

  jornadas=jornadas/simulaciones;


  cout << "Rent10: " << (double)rent10/simulaciones << " Rent50: " << (double)rent50/simulaciones << " Rent100: " << (double)rent100/simulaciones << endl;

  return;
}

bool calcular_corte(){
  int ncorte;
  int ndistintas;
  int icorte[14];
  double *probscorte;
  bool *estan;
  string col;

  linea="corte.txt";
  fin.open(linea.c_str());
  if(fin.is_open()){
//    cout << "Empiezo calculo del corte." << endl;

    getline(fin,linea);
    fin.close();

    ncorte=0;
    for(int i=0; i<14; i++){
      if(linea[i]=='X'){
        icorte[ncorte]=i;
        ncorte++;
      }
    }

    ndistintas=pow(3,ncorte);
    //cout << ndistintas << endl;

    probscorte=new double[ndistintas];
    estan=new bool[ndistintas];
    double ptotal=0;
    for(int i=0; i<ndistintas; i++){
      estan[i]=false;
      linea=intosn(i,ncorte);
      probscorte[i]=1;
      for(int j=0; j<ncorte; j++){
        switch(linea[j]){
          case '1':{ probscorte[i]*=porcentajes[icorte[j]][0]; break;}
          case 'X':{ probscorte[i]*=porcentajes[icorte[j]][1]; break;}
          case '2':{ probscorte[i]*=porcentajes[icorte[j]][2]; break;}
        }
      }
      //cout << probscorte[i] << endl;
      ptotal+=probscorte[i];
    }
    //cout << ptotal << " TOTAL" << endl;

    for(int i=0; i<n_col; i++){
      linea="";
      for(int j=0; j<ncorte; j++){
        linea+=colarchivo[i][icorte[j]];
      }
      //cout << linea << endl;
      estan[stoinn(linea,ncorte)]=true;
      //cout << stoinn(linea,ncorte) << endl;
    }

    pcorte=0;
    for(int i=0; i<ndistintas; i++){
      pcorte+=probscorte[i]*estan[i];
    }

    return true;
  }else{
    return false;
  }

}

int main(int argc, char* argv[])
{
    if(not leer_porcentajes()) return 0;
    if(not leer_apostados()) return 0;

    nombre_archivo=argv[1];
    if(not leer_recaudacion()){
      cout << "Recaudacion esperada en la jornada? ";
      cin >> recaudacion;
    }

    if(nombre_archivo[0]=='-'){
      nombre_archivo=argv[2];
      leer_columnas();
      calcular_todos();
      crear_indices();
      calcular_escrut();
      if(2*emtotal/n_col<1.1){CORTATREN=true;}{  simulacion_tren(); sacar_tren();}
      if(CORTATREN) cout << "TREN CORTADO" << endl;
      return 0;
    }

    if(nombre_archivo[0]=='s'){
      nombre_archivo=argv[2];
      leer_columnas();
      calcular_todos();
      crear_indices();
      calcular_escrut();
      especial_tren();
      sacar_tren();
      return 0;
    }
    if(nombre_archivo[0]=='f'){FULLPREMIOS=true; nombre_archivo=argv[2];}

    { 
      leer_columnas();

      calcular_todos();

      crear_indices();

      calcular_escrut();

      corteok=calcular_corte();

      sacar_resultados();

      char c;

      cout << "Simulacion tren? ";
      cin >> c;

      if(c=='s') {if(2*emtotal/n_col<1.1){CORTATREN=true;}{  simulacion_tren(); sacar_tren();}}
      if(CORTATREN) cout << "TREN CORTADO" << endl;

      cout << "Grafico jornada? ";
      cin >> c;

      if(c=='s') sacar_grafico();

      cout << "Grafico temporada? ";
      cin >> c;

      if(c=='s') sacar_grafico2();

      cout << "Grafico 10 temporadas? ";
      cin >> c;

      if(c=='s') sacar_grafico3();

      return 0;
    }
}
