#include "ampl/ampl.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <queue>
#include "myUsage.h"
using namespace std;
void sort_array(double* &influence, int* &id, int size){
  for(int i=size-1;i>0;i--){
    for(int j=0;j<i;j++){
      if(influence[j]>influence[j+1]){
        double temp;
        int temp_id;
        temp = influence[j+1];
        influence[j+1] = influence[j];
        influence[j] = temp;
        temp_id = id[j+1];
        id[j+1] = id[j];
        id[j] = temp_id;
      }
    }
  } 
}
void sort_array_larger(double* &influence, int* &id, int size){
  for(int i=size-1;i>0;i--){
    for(int j=0;j<i;j++){
      if(influence[j]<influence[j+1]){
        double temp;
        int temp_id;
        temp = influence[j+1];
        influence[j+1] = influence[j];
        influence[j] = temp;
        temp_id = id[j+1];
        id[j+1] = id[j];
        id[j] = temp_id;
      }
    }
  } 
}
double ***build_3D_array(int m, int n, int k);
double  clear_space(string s);
int count_space(string s);


void read_dat( int &unum, int &inum, int &pnum, vector<vector<vector<double> > > &social_aware_weight, vector<vector<double> > &preference, int argc, char** argv){
  std::string dataDirectory = argv[1];
  string tempdata = dataDirectory;
  //clock_t st1,st2,ed1,ed2;
  //st1 = clock(); 
  fstream fin1;
  fin1.open(tempdata.c_str());
  string temp, subtemp;
  unum = 1;
  inum = 1;
  pnum = 0;
  while(getline(fin1, temp, '\n')){
    subtemp = temp.substr(0, temp.find(" "));
    temp.erase(0, subtemp.length()+1);
    if(subtemp == "set"){
      subtemp = temp.substr(0, temp.find(" "));
      temp.erase(0, subtemp.length()+1);
      if(subtemp == "User:="){
        while(temp.find(" ")!=string::npos){
          subtemp = temp.substr(0,temp.find(" "));
          temp.erase(0, subtemp.length()+1);
          unum = unum + 1;
        }
      }
      else if(subtemp == "Item:="){
        while(temp.find(" ")!=string::npos){
          subtemp = temp.substr(0,temp.find(" "));
          temp.erase(0, subtemp.length()+1);
          inum = inum + 1;
        }
      }
    }
    else if(subtemp == "param"){
      subtemp = temp.substr(0, temp.find(" "));
      temp.erase(0, subtemp.length()+1);
      if(subtemp == "pos:="){
        subtemp = temp.substr(0, temp.find(";"));
        pnum = atoi(subtemp.c_str());
      }
      else if(subtemp == "social_aware_weight:="){

        if(pnum == 0)break;
        else{
          for(int i=0; i<inum; i++){
            vector<vector<double> > temp1;
            for(int j=0; j<unum; j++){
              vector<double> temp;
              for(int k=0; k<unum; k++){
                double a = 0.0;
                temp.push_back(a);
              }
              temp1.push_back(temp);
            }
            social_aware_weight.push_back(temp1);
          }
          int fac = unum * unum;
          for(int i=0; i<inum; i++){
            getline(fin1, temp, '\n');
            for(int j=0; j<fac; j++){
              getline(fin1, temp, '\n');
              int u1,u2;
              double r;
              subtemp = temp.substr(0, temp.find(" "));
              temp.erase(0, subtemp.length()+1);
              subtemp.erase(0,1);
              u1 = atoi(subtemp.c_str());
              subtemp = temp.substr(0, temp.find(" "));
              temp.erase(0, subtemp.length()+1);
              subtemp.erase(0,1);
              u2 = atoi(subtemp.c_str());
              if(i!=unum*(unum-1)-1){
                subtemp = temp;
                r = atof(subtemp.c_str());
              }
              else if(i==unum*(unum-1)-1){
                subtemp = temp.substr(0, temp.find(";"));
                r = atof(subtemp.c_str());
              }
              social_aware_weight[i][u1][u2] = r;
            }
          }
        }
      }
      else if(subtemp == "preference:="){
        for(int i=0;i<inum;i++){
          vector<double> temp1;
          for(int j=0; j<unum; j++){
            temp1.push_back(0.0);
          }
          preference.push_back(temp1);
        }
        for(int i=0;i<inum;i++){
          getline(fin1, temp,'\n');
          if(temp.find("[")!= string::npos){
            for(int j=0;j<unum;j++){
              int u1;
              double p;
              getline(fin1, temp, '\n');
              subtemp = temp.substr(0, temp.find(" "));
              temp.erase(0, subtemp.length()+1);
              subtemp.erase(0,1);
              u1 = atoi(subtemp.c_str());
              if(i != inum - 1){
                subtemp = temp.substr(0 , temp.find("\n"));
                p = atof(subtemp.c_str());
              }
              else if(i == inum -1){
                subtemp = temp.substr(0, temp.find(";"));
                p = atof(subtemp.c_str());
              }
              preference[i][u1] = p;
            }
          }
        }
      }
    }
  }
  cout<<"unum: "<<unum<<" inum: "<<inum<<" pnum: "<<pnum<<endl;
}
void solve(double ***X, int unum, int inum, int pnum, int argc, char **argv){

  //time limit and thread count
  string threads = "1";
  string timelim = "100000";

  if (argc > 2){
    threads = argv[2];
  }
  if (argc > 3){
    timelim = argv[3];
  }

  ampl::AMPL ampl;
  ampl.setOption("solver", "gurobi_ampl");

  //set gurobi options
  string gurobi_option_string = "option gurobi_options 'barconvtol=1e-1 crossover=0 method=2 outlev=1 timing=1 threads=";
  gurobi_option_string += threads;
  gurobi_option_string += " timelim=";
  gurobi_option_string += timelim;
  gurobi_option_string += "';";
  ampl.eval(gurobi_option_string);

  ampl.read("VRPR_ilp_new.mod");
  ampl.readData(argv[1]);
  ampl.solve();
  std::cout<<"Objective Maximize: "<<ampl.getObjective("VRPR").value()<<"\n";
  ampl::DataFrame x = ampl.getVariable("x").getValues();
  string strx = x.toString();
  stringstream ss(strx);
  string temp2,subtemp2;
  getline(ss, temp2, '\n');  
  
  for(int i=0;i<unum;i++){
    for(int j=0;j<pnum;j++){
      for(int k=0;k<inum;k++){
        getline(ss, temp2, '\n');
        int U,P,I;
        int space = count_space(temp2);
        temp2.erase(0,space+1);
        subtemp2 = temp2.substr(0, temp2.find("'"));
        temp2.erase(0,subtemp2.length()+1);
        subtemp2 = subtemp2.erase(0,1);
        U = atoi(subtemp2.c_str());
        //cerr<<"U:"<<U<<" ";
        space = count_space(temp2);
        temp2.erase(0, space);
        subtemp2 = temp2.substr(0, temp2.find(" "));
        temp2.erase(0, subtemp2.length());
        P = atoi(subtemp2.c_str()) - 1;
        //cerr<<"P:"<<P<<" ";
        space = count_space(temp2);
        temp2.erase(0, space+1);
        subtemp2 = temp2.substr(0, temp2.find("'"));
        temp2.erase(0, subtemp2.length()+1);
        subtemp2.erase(0,1);
        I = atoi(subtemp2.c_str());
        //cerr<<"I:"<<I<<" ";
        subtemp2 = temp2.substr(0, temp2.find("|"));
        temp2.erase(0, subtemp2.length()+1);
        X[U][P][I] = clear_space(temp2);
        //cerr<<X[U][P][I]<<endl;
      }
    }
  }
  /*for(int i=0;i<unum;i++){
    cout<<"sdfghjklkjhgfdfghjklkjhgf"<<endl;
    for(int j=0;j<pnum;j++){
      for(int k=0;k<inum;k++){
        cout<<X[i][j][k]<<" ";
      }
      cout<<endl;
    }
    cout<<endl;
  }*/
}
void random_algorithm(vector<vector<int> > &result, double ***X, vector<vector<vector<double> > > social_aware_weight, vector<vector<double> > preference, double* objective, int unum, int inum, int pnum){
  for(int i=0;i<unum;i++){
    vector<int> temp;
    for(int j=0;j<pnum;j++){
      temp.push_back(-1);
    }
    result.push_back(temp);
  }
  vector<vector<bool> > item_used;
  for(int i=0;i<unum;i++){
    vector<bool> temp;
    for(int j=0;j<inum;j++){
      temp.push_back(0);
    }
    item_used.push_back(temp);
  }
  int total_space = (unum * pnum) ;
  int tempnum = 0;
  unsigned seed;
  seed = (unsigned)time(NULL);
  srand(seed);
  int random_p, random_i;
  double random_alpha;
  //st2 = clock();
  bool inv = 0;
  int temp_total_space,count;
  while(tempnum < total_space){//random分配
    if(temp_total_space == tempnum){
      count = count + 1;
    }
    else{
      count = 0;
    }
    if(count > 10000){
      inv = 1;
    }
    temp_total_space = tempnum;
    random_p = rand() % (pnum);
    random_i = rand() % (inum);
    //cout<<count<<endl;
    if(inv == 0 )random_alpha = (double)rand() / (RAND_MAX+1.0);
    else random_alpha = 0;
    //cout<<random_p<<" "<<random_i<<endl;
    for(int i=0;i<unum;i++){ 
      if(X[i][random_p][random_i] >= random_alpha && result[i][random_p] == -1 && item_used[i][random_i] == 0){
        result[i][random_p] = random_i;
        item_used[i][random_i] = 1;
        tempnum = tempnum + 1;
      }
    }
    /*for(int i=0;i<unum;i++){
      for(int j=0;j<inum;j++){
        cout<<item_used[i][j]<<" ";
      }
      cout<<endl;
    }*/
    //cout<<tempnum<<endl;

  }
  //objective calculating
  for(int i=0;i<unum;i++){
    for(int j=0;j<pnum;j++){
      objective[0] = objective[0] + preference[result[i][j]][i];
      objective[1] = objective[1] + preference[result[i][j]][i];
      for(int k=0;k<unum;k++){
        if(i!=k && result[i][j] == result[k][j]){
          objective[0] = objective[0] + social_aware_weight[result[i][j]][i][k];
          objective[2] = objective[2] + social_aware_weight[result[i][j]][i][k];
        }
      }
    }
  }
}
//void average_social_degree()
//average shared people per item
void av_shared_people_per_item(vector<vector<int> > result, int unum, int inum, int pnum){
  double av_shared_people_per_item = 0;
  vector<int> c_item;
  vector<int> c_score;
  for(int i=0;i<unum;i++){
    for(int j=0;j<pnum;j++){
      bool exsist = 0;
      for(int n=0;n<c_item.size();n++){
        if(result[i][j] == c_item.at(n)){
          c_score.at(n) = c_score.at(n) + 1;
          exsist = 1;
          break;
        }
      }
      if(exsist == 0){
        c_score.push_back(1);
        c_item.push_back(result[i][j]);
      }
    }
  }
  for(int i=0;i<c_item.size();i++){
    av_shared_people_per_item = av_shared_people_per_item + c_score.at(i);
  }
  av_shared_people_per_item = av_shared_people_per_item / (double)c_item.size();
  cout<<"shared people per item: "<<av_shared_people_per_item<<endl;
}
void av_shared_item_per_friend_pair(vector<vector<int> > result, int unum, int inum, int pnum){
  //average shared items per friend pair
  double fp = 0;
  for(int i=0;i<unum;i++){
    for(int j=0;j<pnum;j++){
      for(int n=0;n<unum;n++){
        if(i!=n && result[i][j] == result[n][j]){
          fp++;
        }
      }
    }
  }
  fp = fp / (double)((unum * (unum - 1)));
  cout<<"shared item per friend pair: "<<fp<<endl;
}
//average preference score per item
void av_pref_sc_per_item(vector<vector<int> > result, vector<vector<double> > preference, int unum, int inum, int pnum){
  vector<double> pref_score_item, pref_i;
  for(int i=0;i<inum;i++){
    pref_score_item.push_back(0);
    pref_i.push_back(0);
  }
  for(int i=0;i<unum;i++){
    for(int j=0;j<pnum;j++){
      pref_score_item[result[i][j]] = pref_score_item[result[i][j]] + preference[result[i][j]][i];
      pref_i[result[i][j]]++;
    }
  }
  for(int i=0;i<inum;i++){
    //if(pref_i[i] != 0){pref_score_item[i] = pref_score_item[i] / (double)pref_i[i];}
  }
  cout<<"preference score per item: "<<endl;
  for(int i=0;i<inum;i++){
    cout<<"I"<<i<<": "<<pref_score_item[i]<<endl;
  }
  cerr<<"sdfasdff"<<endl;
}
//average preference score per people
void av_pref_sc_per_person(vector<vector<int> > result, vector<vector<double> > preference, int unum, int inum, int pnum){ 
  vector<double> pref_score_person, pref_p;
  cerr<<"in here"<<endl;
  for(int i=0;i<unum;i++){
    pref_score_person.push_back(0.0);
    pref_p.push_back(0.0);
  }
  for(int i=0;i<unum;i++){
    for(int j=0;j<pnum;j++){
      pref_score_person[i] = pref_score_person[i] + preference[result[i][j]][i];
      pref_p[i]++;
    }
  }
  for(int i=0;i<unum;i++){
    //pref_score_person[i] = pref_score_person[i] / (double)pref_p[i];
  }
  cout<<"preference score per person: "<<endl;
  for(int i=0;i<unum;i++){
    cout<<"U"<<i<<": "<<pref_score_person[i]<<endl;
  }
}
void result_p(vector<vector<int> > result, int unum, int inum, int pnum){
  cout<<"result: position(xlabel), user(ylabel)"<<endl;
  for(int i=0;i<unum;i++){
    cout<<"U"<<i<<" ";
    for(int j=0;j<pnum;j++){
      cout<<result[i][j]<<" ";
    }
    cout<<endl;
  }
}
void Xrate(double ***X, int unum, int inum, int pnum){
  double zero=0,one=0,none_one=0;
  double total = unum * inum * pnum;
  cout<<total<<" "<<unum<<" "<<inum<<" "<<pnum<<endl;
  for(int i=0;i<unum;i++){
    for(int j=0;j<pnum;j++){
      for(int k=0;k<inum;k++){
        if(X[i][j][k] == 1){one++;}
        else if (X[i][j][k] == 0) {zero++;}
        else {none_one++;}
        
      }
    }
  }
  //cout<<"here"<<endl;
  cout<<"zero rate   : "<<zero / total<<endl;
  cout<<"one rate    : "<<one / total<<endl;
  cout<<"(0..1) rate : "<<none_one / total<<endl;
}
void print_summary(vector<vector<int> > result, vector<vector<double> > preference, double* objective, double ***X, int unum, int inum, int pnum){
  cout<<endl;
  cout<<"Summary"<<endl;
  cout<<"=============================================================="<<endl;
  cout<<"Objective: "<<objective[0]<<endl;
  cout<<"Preference: "<<objective[1]<<endl;
  cout<<"Social: "<<objective[2]<<endl;
  cout<<"=============================================================="<<endl;
  av_shared_people_per_item(result, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  av_shared_item_per_friend_pair(result, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  av_pref_sc_per_item(result, preference, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  av_pref_sc_per_person(result, preference, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  result_p(result, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  Xrate(X, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
}
int main(int argc, char **argv) {
    MyUsage myUsage;
    int inum, unum, pnum;
    vector<vector<vector<double> > > social_aware_weight;
    vector<vector<double> > preference;
    vector<vector<int> > result;
    read_dat(unum, inum, pnum, social_aware_weight, preference, argc, argv);
    double ***X = build_3D_array(unum, pnum, inum);
    solve(X, unum, inum, pnum, argc, argv);
    double  objective[3];
    for(int i=0;i<3;i++){
      objective[i] = 0;
    }
    random_algorithm(result, X, social_aware_weight, preference, objective, unum, inum, pnum);
    myUsage.report(1,1);
    print_summary(result, preference, objective, X, unum, inum, pnum);
}

double ***build_3D_array(int m, int n, int k){
  double ***A = new double**[m];
  for(int i=0;i<m;i++){
    A[i] = new double*[n];
    for(int j=0;j<n;j++){
      A[i][j] = new double[k];
    }
  }
  for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
      for(int h=0;h<k;h++){
        A[i][j][h] = 0;
      }
    }
  }
  return A;
}
double clear_space(string s){
  string temp;
 // cout<<"s: "<<s<<" ";
  int pos;
  for(int i=0;i<s.length();i++){
    if(s[i]!=' '){
      pos = i;
     // cout<<pos<<endl;
      break;
    }
  }
  s.erase(0, pos);
  temp = s.substr(0, s.find(" "));
  double r = atof(temp.c_str());
 // cout<<"r: "<<r<<endl;
  return r;
}

int count_space(string s){
  int pos;
  for(int i=0;i<s.length();i++){
    if(s[i]!=' '){
       pos = i;
       break;
    }
  }
  return pos;
}

