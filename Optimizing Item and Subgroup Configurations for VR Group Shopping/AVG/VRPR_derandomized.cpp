#include "ampl/ampl.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <math.h>
#include <queue>
#include "myUsage.h"
#include "social_output.h"
#include <omp.h>
using namespace std;
string int2str(int &i){
  string s;
  stringstream ss(s);
  ss<<i;
  return ss.str();
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

vector<vector<int> > sort_array(vector<vector<double> > pref, int unum, int inum, int pnum){
  vector<vector<int > > id;
  for(int i=0;i<unum;i++){
    vector<int > temp_i;
    for(int j=0;j<inum;j++){
      temp_i.push_back(j);
    }
    id.push_back(temp_i);
  }
  vector<vector<double> > temp_p;
  for(int i=0;i<unum;i++){
    vector<double> p;
    for(int j=0;j<inum;j++){
      p.push_back(pref[j][i]);
    }
    temp_p.push_back(p);
  }
  for(int i=0;i<unum;i++){
    for(int j=0;j<inum;j++){
      for(int k=0;k<(inum-j-1);k++){
        if(temp_p[i][k]<temp_p[i][k+1]){
          double temp;
          int temp_id;
          temp = temp_p[i][k+1];
          temp_p[i][k+1] = temp_p[i][k];
          temp_p[i][k] = temp;
          temp_id = id[i][k+1];
          id[i][k+1] = id[i][k];
          id[i][k] = temp_id;
        }
      }
    }
  } 
  return id;
}



void read_dat( int &unum, int &inum, int &pnum, vector<vector<vector<double> > > &social_aware_weight, vector<vector<double> > &preference, float lambda, int argc, char **argv){
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
              social_aware_weight[i][u1][u2] = r * lambda;
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
              preference[i][u1] = p * (1-lambda);
            }
          }
        }
      }
    }
  }
  cout<<"unum: "<<unum<<" inum: "<<inum<<" pnum: "<<pnum<<endl;
}

vector<int> filter(vector<vector<double > > preference, vector<vector<vector<double > > > social_aware_weight, int unum, int inum, int pnum, int th,char** argv){
  vector<vector<int > > sort_pref = sort_array(preference, unum, inum, pnum);
  vector<bool > in_bool;
  vector<int > in_item;
  int fil_i = 0;
  for(int i=0;i<inum;i++){
    in_bool.push_back(0);
  }
  for(int i=0;i<unum;i++){
    for(int j=0;j<th;j++){
      in_bool[sort_pref[i][j]] = 1;
    }
  }
  for(int i=0;i<inum;i++){
    if(in_bool[i] == 1){
      fil_i++;
      in_item.push_back(i);
    }
  }
  string fil = argv[1];
  fil = fil.substr(0, fil.find(".dat"));
  fil = fil + "_filter.dat";  
  ofstream output(fil.c_str());
  output << "set User:= ";
  for(int i=0;i<unum;i++){
    output<<"U"<<int2str(i)<<" ";
  }
  output<<";"<<endl;
  output<<"set Item:= ";
  for(int i=0;i<inum;i++){
    if(in_bool[i] == 1){
      output << "I"<<int2str(i)<<" ";
    }
  }
  output<<";"<<endl;
  output<<"param pos:= "<<pnum<<";"<<endl; //original setting
  if (atof(argv[6]) > 0.){
    output << "param discount:= " << argv[6] << ";" << endl;
  } 
  output<<"param social_aware_weight:= "<<endl;
  for(int i=0;i<inum;i++){
    if(in_bool[i] == 1){
      output<<"[*,*,I"<<int2str(i)<<"]"<<endl;
      for(int j=0;j<unum;j++){
        for(int k=0;k<unum;k++){
          output<<"U"<<int2str(j)<<" "<<"U"<<int2str(k)<<" "<<social_aware_weight[i][j][k]<<endl;
        }
      }
    }
  }
  output<<";"<<endl;
  output<<"param preference:= "<<endl;
  for(int i=0;i<inum;i++){
    if(in_bool[i] == 1){
      output<<"[I"<<int2str(i)<<",*]"<<endl;
      for(int j=0;j<unum;j++){
        output<<"U"<<int2str(j)<<" "<<preference[i][j]<<endl;
      }
    }
  }
  output<<";";
  return in_item;
}

void solve(vector<vector<double> > preference, vector<vector<vector<double> > > social_aware_weight, double ***X, vector<vector<vector<double > > > &X_fil, vector<int> &in_item, int unum, int inum, int pnum, int argc, char **argv){
  ampl::AMPL ampl;
  ampl.setOption("solver", "gurobi_ampl");

  string threads = "1";
  string timelim = "100000";
  string data = argv[1];
  vector<int> fil_item;
  int fil_i = inum;
  bool precise_mode = false;

  if (argc > 2){
   if (atoi(argv[2])){
     precise_mode = true;
   }
  }

  if (argc > 3){
    threads = argv[3];
  }
  if (argc > 4){
    timelim = argv[4];
  }
  if (argc > 5){
    data = data.substr(0,data.find(".dat"));
    data = data + "_filter.dat";
    string fil_str = argv[5];
    int fil = atoi(fil_str.c_str());
    fil_item = filter(preference, social_aware_weight, unum, inum, pnum, fil, argv);
    fil_i = fil_item.size();
    in_item = fil_item;
  }
  else{
    for(int i=0;i<inum;i++){
      fil_item.push_back(i);
    }
    in_item = fil_item;
  }

  string ampl_solver;
  if (argc > 6){
    if (atof(argv[6]) == 0.){
      ampl_solver = "VRPR_lp_reduced.mod";
    }
    else if (atof(argv[6]) > 0.){
      ampl_solver = "VRPR_lp_unaligned_reduced.mod";
    }
    else{
      cout << "Wrong discout input" << endl;
    }
 }

  string gurobi_option_string;
  if(precise_mode){
    gurobi_option_string = "option gurobi_options 'barconvtol=1e-4 crossover=0 method=2 outlev=1 timing=1 threads=";
  }
  else{
    gurobi_option_string = "option gurobi_options 'barconvtol=1e-1 crossover=0 method=2 outlev=1 timing=1 threads=";
  }

  gurobi_option_string += threads;
  gurobi_option_string += " timelim=";
  gurobi_option_string += timelim;
  gurobi_option_string += "';";
  ampl.eval(gurobi_option_string);

  ampl.read(ampl_solver);
  ampl.readData(data);
  ampl.solve();
  std::cout<<"Objective Maximize: "<<ampl.getObjective("VRPR").value()<<"\n";
  ampl::DataFrame x = ampl.getVariable("x").getValues();
  string strx = x.toString();
  stringstream ss(strx);
  string temp2,subtemp2;
  getline(ss, temp2, '\n');  
  for(int i=0;i<unum;i++){
    //for(int j=0;j<pnum;j++){
      for(int k=0;k<fil_i;k++){
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
    //}
  }

  float max_X = 0.0;

  for(int i=0;i<unum;i++){
    vector<vector<double > > temp1;
    for(int j=0;j<pnum;j++){
      vector<double > temp2;
      for(int k=0;k<inum;k++){
        for(int n=0;n<fil_i;n++){
          if(k == fil_item[n]){
            if(j==0){
              X[i][j][k] = X[i][j][k]/pnum;
              if (X[i][j][k] > max_X){
                max_X = X[i][j][k];
              }
            }
            temp2.push_back(X[i][0][k]);
            break;
          }
        }
      }
      temp1.push_back(temp2);
    }
    X_fil.push_back(temp1);
  }

for(int i=0; i<unum;i++){
  for(int j=0;j<pnum;j++){
    for(int k=0; k<fil_i;k++){
      X_fil[i][j][k] = X_fil[i][0][k];
    }
    for(int k=0; k<inum; k++){
      X[i][j][k] = X[i][0][k];
    }
  }
}

//cout << "max_X = " << max_X << endl;
}
void derandom_algorithm(vector<vector<int> > &result, double*** X, vector<int> in_item, vector<vector<vector<double> > > social_aware_weight, vector<vector<double> > preference, double* objective ,int unum, int inum, int pnum , double future_factor){
  vector<double> object;
  for(int i=0;i<pnum;i++)object.push_back(0);

  for(int i=0;i<pnum;i++){
    for(int j=0;j<unum;j++){
      for(int k=0;k<unum;k++){
        for(int l=0;l<in_item.size();l++){
          if(j != k){
            double temp;
            if(X[j][i][in_item[l]] <= X[k][i][in_item[l]])temp = X[j][i][in_item[l]];
            else temp = X[k][i][in_item[l]];
            object[i] = object[i] + social_aware_weight[in_item[l]][j][k] * temp;
            if(j==k){
              object[i] = object[i] + preference[in_item[l]][j] * X[j][i][in_item[l]];
            }
          }          
        }
      }
    }
  }
  for(int i=0;i<pnum;i++){
    object[i] = future_factor * object[i];
  }
  int total_space = (unum * pnum);
  int tempnum = 0;
  vector<vector<int> > choose;
  vector<vector<bool> > user_item;
  vector<int> full;
  for(int i=0;i<unum;i++){
    vector<int> temp;
    for(int j=0;j<pnum;j++){
      temp.push_back(-1);
    }
    choose.push_back(temp);
    result.push_back(temp);
  }
  for(int i=0;i<unum;i++){
    vector<bool> temp;
    for(int j=0;j<inum;j++){
      temp.push_back(0);
    }
    user_item.push_back(temp);
  }
  for(int i=0;i<inum;i++)full.push_back(0);
  //st2 = clock();
  while(tempnum < total_space){
    cout<<tempnum<<" / "<<total_space<<endl;


    double best_value = 0;
    //cerr<<best_value<<" ";
    double best_config[3];//i,p,a
    vector<int> c_user;//user be picked to change this time
    double temp_object;
    #pragma omp parallel for
    for(int i=0;i<in_item.size();i++){
      if(full[in_item[i]] < unum){
        for(int j=0;j<pnum;j++){
          double temp_best = 0;
          for(int k=0;k<pnum;k++){
            if(k != j)temp_best = temp_best + object[k];
          }
          for(int k=0;k<unum;k++){
            double alpha = X[k][j][in_item[i]];
            double temp_alg = 0;
            double temp_opt = 0;
            vector<int> choose_user;
            vector<int> not_choose_user;
            int choose_total = 0;
            int not_choose_total = 0;
            for(int n=0;n<unum;n++){
              if(X[n][j][in_item[i]] >= alpha && choose[n][j] == -1){
                choose_user.push_back(n);
                choose_total += 1;
              }
              else if(choose[n][j] != 1){
                not_choose_user.push_back(n);
                not_choose_total += 1;
              }
            }
            //alg part
            for(int n=0;n<choose_total;n++){
              for(int o=0;o<choose_total;o++){
                if(n != o)temp_alg = temp_alg + social_aware_weight[in_item[i]][choose_user.at(n)][choose_user.at(o)];
                else if(n==o)temp_alg = temp_alg + preference[in_item[i]][choose_user.at(n)];
              }
            }
            //opt part
            for(int n=0;n<not_choose_total;n++){
              for(int o=0;o<not_choose_total;o++){
                for(int p=0;p<in_item.size();p++){
                  if(n != o){
                    if(X[n][j][p] <= X[o][j][p]){
                      temp_opt = temp_opt + future_factor * social_aware_weight[in_item[p]][not_choose_user.at(n)][not_choose_user.at(o)] * X[n][j][in_item[p]];
                    }
                    else{
                      temp_opt = temp_opt + future_factor * social_aware_weight[in_item[p]][not_choose_user.at(n)][not_choose_user.at(o)] * X[o][j][in_item[p]];
                    }
                  }
                  if(n == o)temp_opt = temp_opt + future_factor * preference[in_item[p]][not_choose_user.at(n)] * X[not_choose_user.at(n)][j][in_item[p]];
                }
              }
            }
            if(temp_best + temp_alg + temp_opt  > best_value && temp_alg > 0){
              best_value = temp_best + temp_alg + temp_opt;
              best_config[0] = in_item[i];
              best_config[1] = j;
              best_config[2] = X[k][j][in_item[i]];
              c_user = choose_user;
              temp_object = temp_alg + temp_opt;
              //cerr<<i<<":"<<best_value<<" "<<best_config[2]<<endl;
            }
            //cout<<choose_user.size()<<endl;
          }
        }
      }
    }
    int changed_result = 0;
    for(int j=0;j<c_user.size();j++){
      if(user_item[c_user.at(j)][(int)best_config[0]] == 0){
        choose[c_user.at(j)][(int)best_config[1]] = 1;
        result[c_user.at(j)][(int)best_config[1]] = (int)best_config[0];
        user_item[c_user.at(j)][(int)best_config[0]] =1;
        changed_result += 1;
      }
    }
    object[(int)best_config[1]] = temp_object;
    tempnum += changed_result;
    full[(int)best_config[0]] += c_user.size();
  }
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
    if(pref_i[i] != 0){pref_score_item[i] = pref_score_item[i] / (double)pref_i[i];}
  }
  cout<<"preference score per item: "<<endl;
  for(int i=0;i<inum;i++){
    cout<<"I"<<i<<": "<<pref_score_item[i]<<endl;
  }
}
//average preference score per people
void av_pref_sc_per_person(vector<vector<int> > result, vector<vector<double> > preference, int unum, int inum, int pnum){ 
  vector<double> pref_score_person, pref_p;
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
    pref_score_person[i] = pref_score_person[i] / (double)pref_p[i];
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
  for(int i=0;i<unum;i++){
    for(int j=0;j<pnum;j++){
      for(int k=0;k<inum;k++){
        if(X[i][j][k] == 1)one++;
        else if (X[i][j][k] == 0) zero++;
        else none_one++;
      }
    }
  }
  cout<<"zero rate   : "<<zero / total<<endl;
  cout<<"one rate    : "<<one / total<<endl;
  cout<<"(0..1) rate : "<<none_one / total<<endl;
}
void print_summary(vector<vector<int> > result, vector<vector<vector<double> > > social_aware_weight, vector<vector<double> > preference, double* objective, double ***X, int unum, int inum, int pnum){
  cout<<endl;
  cout<<"Summary"<<endl;
  cout<<"=============================================================="<<endl;
  cout<<"Objective: "<<objective[0]<<endl;
  cout<<"Prefernce: "<<objective[1]<<endl;
  cout<<"Social: "<<objective[2]<<endl;
  
  vector<pair<int, int> > social_graph;
  Subgroup_output sout;
  sout.derive_social_graph(social_graph, social_aware_weight, unum, inum, 0);
  cout<<"=============================================================="<<endl;
  av_shared_people_per_item(result, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  sout.av_shared_item_per_friend_pair(social_graph, result, unum, inum, pnum);
  //cout<<"=============================================================="<<endl;
  //av_pref_sc_per_item(result, preference, unum, inum, pnum);
  //cout<<"=============================================================="<<endl;
  //av_pref_sc_per_person(result, preference, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
   sout.av_density_in_group(result, social_graph, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  sout.regret(social_graph, result, preference, social_aware_weight, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  result_p(result, unum, inum, pnum);
  //cout<<"=============================================================="<<endl;
  //Xrate(X, unum, inum, pnum);
  //cout<<"=============================================================="<<endl;
}
int main(int argc, char **argv) {
  
  MyUsage myUsage;
  int inum, unum, pnum;
  vector<vector<vector<double> > > social_aware_weight, X_fil;
  vector<vector<double> > preference;
  vector<vector<int> > result;
  vector<int> in_item;

  float lambda = 0.5;
  double future_factor;
  if (argc > 7){
    lambda = atof(argv[7]);
    future_factor = atof(argv[8]);
  }

  read_dat(unum, inum, pnum, social_aware_weight, preference, lambda, argc, argv);
  double ***X = build_3D_array(unum, pnum, inum);
  solve(preference, social_aware_weight, X, X_fil, in_item, unum, inum, pnum, argc, argv);
  double objective[3];
  for(int i=0;i<3;i++){
    objective[i] = 0;
  }

  time_t timer1 = time(NULL);

  derandom_algorithm(result, X, in_item, social_aware_weight, preference, objective , unum, inum, pnum , future_factor);

  time_t timer2 = time(NULL);
  float timedif;
  cout << "Derandomized Time Usage: " << difftime(timer2, timer1) << " Seconds. " << endl;
  //myUsage.report(1,1);
  print_summary(result, social_aware_weight, preference, objective, X, unum, inum, pnum);
}

