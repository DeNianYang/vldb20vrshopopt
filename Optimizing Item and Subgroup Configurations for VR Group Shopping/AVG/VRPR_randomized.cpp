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
#include <algorithm>
#include "social_output.h"
using namespace std;
double ***build_3D_array(int m, int n, int k);
double  clear_space(string s);
int count_space(string s);
string int2str(int &i){
  string s;
  stringstream ss(s);
  ss<<i;
  return ss.str();
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


void read_dat( int &unum, int &inum, int &pnum, vector<vector<vector<double> > > &social_aware_weight, vector<vector<double> > &preference, float lambda, int argc, char** argv){
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
              preference[i][u1] = p * (1 - lambda);
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
  if (atof(argv[6]) > 0.)
  {
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

float solve(vector<vector<double> > preference, vector<vector<vector<double> > > social_aware_weight, double ***X, vector<vector<vector<double > > > &X_fil, vector<int> &in_item,int unum, int inum, int pnum, int argc, char **argv){

  //time limit and thread count
  string threads = "1";
  string timelim = "100000";
  string data = argv[1];
  vector<int> fil_item;
  int fil_i = inum;
  bool precise_mode = false; //default: efficiency-oriented

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
    //if(fil < pnum){
    //  fil = pnum;
    //}
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

  string ampl_model; // decide which model to use
  if (argc > 6){
    if (atof(argv[6]) == 0.){
      ampl_model = "VRPR_lp_reduced.mod";
    }
    else if (atof(argv[6]) > 0.){
      ampl_model = "VRPR_lp_unaligned_reduced.mod";
    }
    else{
      cout << "Wrong discout input" << endl;
    }
    
  }

  ampl::AMPL ampl;
  ampl.setOption("solver", "gurobi_ampl");

  //set gurobi options
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

  //ampl.read("VRPR_lp_reduced.mod");
  ampl.read(ampl_model);
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
  }
}

//cout << "max_X = " << max_X << endl;


return max_X;
}
void random_algorithm(vector<vector<int> > &result, vector<vector<vector<double> > > X, vector<int> in_item, vector<vector<vector<double> > > social_aware_weight, vector<vector<double> > preference, double* objective, int unum, int inum, int pnum, float max_X, char** argv, int group_size_ub){
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
    random_i = rand() % (in_item.size());
    
    if(inv == 0 )random_alpha = (double)rand() / (RAND_MAX+1.0);
    else random_alpha = 0;
    random_alpha *= max_X; //scale down the threshold

    int group_size_count = 0;
    if (group_size_ub < unum){ //check how many users already see this item
      for(int i=0; i<unum; i++){
        if(result[i][random_p] == in_item[random_i]){
          group_size_count++;
        }
      }
    }

    for(int i=0;i<unum;i++){
      if (group_size_count == group_size_ub){
        break; //STOP adding users
      }
      if(X[i][random_p][random_i] >= random_alpha && result[i][random_p] == -1 && item_used[i][in_item[random_i]] == 0){
        result[i][random_p] = in_item[random_i];
        item_used[i][in_item[random_i]] = 1;
        tempnum = tempnum + 1;
        group_size_count++;
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
        for(int t=j+1;t<pnum;t++){
          if (i!=k && result[i][j] == result[k][t]){
            objective[0] = objective[0] + atof(argv[6])*social_aware_weight[result[i][j]][i][k];
            objective[3] = objective[3] + atof(argv[6])*social_aware_weight[result[i][j]][i][k];
          }
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
void av_shared_item_per_friend_pair(vector<pair<int, int> > social_graph, vector<vector<int> > result, int unum, int inum, int pnum){
  /*** Complete graph version**/
  //average shared items per friend pair
  /*double fp = 0;
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
  cout<<"shared item per friend pair: "<<fp<<endl;*/

  /******Social graph version********/
  double items = 0.;
  double n_edge  = 0.;
  
  for(int edge = 0; edge < social_graph.size(); edge++){
    int u1 = social_graph[edge].first;
    int u2 = social_graph[edge].second;
    double temp = 0.;
    for(int k=0;k<pnum;k++){
      if (result[u1][k]==result[u2][k]){
        temp += 1.;
      }
    }
    items += temp;
    n_edge  += 1.;
  }
  cout<<"shared item per friend pair: "<<items/n_edge<<endl;
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
  //cerr<<"in here"<<endl;
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

void derive_social_graph(vector<pair<int, int> > &social_graph, vector<vector<vector<double> > > social_aware_weight, int unum, int inum, bool print){
  int edge_count = 0;
  for(int i = 0;i < unum;i++){
    for(int j = i+1;j < unum; j++){
      for(int k = 0;k < inum;k++){
        if (social_aware_weight[k][i][j] > 0.){
          social_graph.push_back( make_pair(i,j) );
          edge_count++;
          break;
        }
      }
    }
  }
  cout << "Total #users:" << unum << ", total #edges:" << edge_count << endl;

  if (print){
    cout << "Social graph edge list:" << endl;
    for(int i = 0;i < social_graph.size();i++){
      cout << social_graph[i].first << ", " << social_graph[i].second << endl;
    }
  }
}

void av_density_in_group(vector<vector<int> > result, vector<pair<int, int> > social_graph, int unum, int inum, int pnum){
  double total = 0.;
  double inter_intra_ratio = 0.;
  double alone = 0.;
  for(int k = 0;k < pnum;k++){
    vector<int> subgroups(unum, -1);
    int max_group = 0;
    for(int u1=0;u1 < unum;u1++){
      if (subgroups[u1]==-1){
        subgroups[u1] = max_group++;
      }
      for(int u2=u1+1;u2 < unum;u2++){
        if (result[u1][k]==result[u2][k]){
          subgroups[u2] = subgroups[u1];
        }
      }
    }

    double one_slot_total = 0.;
    double intra_edges = 0.;
    for(int g = 0;g < max_group;g++){
      double n_edge = 0.;
      double n_node = 0.;
      for(int u1=0;u1 < unum;u1++){
        if (subgroups[u1] == g){
          n_node += 1.;
        }
        else{
          continue;
        }
        for(int u2=u1+1;u2 < unum;u2++){
          if (subgroups[u2] == g && find(social_graph.begin(), social_graph.end(), make_pair(u1,u2))!=social_graph.end()){
            n_edge += 1.;
          }
        }
      }
      //cout << n_node << " " << n_edge << endl;
      //one_slot_total += n_edge/(n_node-1);
      intra_edges += n_edge;
      if (n_node == 1){
        alone += 1.;
      }
      else {
        one_slot_total += n_edge/(n_node-1);
      }
    }
    total += one_slot_total;
    inter_intra_ratio += (social_graph.size()-intra_edges)/intra_edges;
  }
  //cout << "edges: " << social_graph.size() << endl;
  cout << "Average density among groups: " << 2*total/(unum*pnum) << ", Normalized: "<< (2*total/(unum*pnum)) / (2*double(social_graph.size())/(unum*(unum-1))) << endl;
  cout << "Inter/intra ratio: " << inter_intra_ratio/pnum << endl;
  cout << "Along rate: " << alone/pnum << ", along number: "<< alone << endl;
}

void regret(vector<pair<int, int> > social_graph, vector<vector<int> > result, vector<vector<double> > preference, vector<vector<vector<double> > > social_aware_weight, int unum, int inum, int pnum){
  vector<double> maximum(unum, 0.);
  vector<double> objective(unum, 0.);
  vector<vector<double> > max_heaps(unum);
  vector<vector<int> > sg_adj(unum, vector<int>(unum));

  //build social_graph
  /*for(int i = 0;i < unum;i++){
    sg_adj.push_back( vector<int> temp(unum, 0) );
  }*/
  for(int i = 0;i < social_graph.size();i++){
    pair<int, int> e = social_graph[i];
    sg_adj[e.first][e.second] = 1;
    sg_adj[e.second][e.first] = 1;
  }
  
  //build max_heaps
  for(int k = 0;k < inum;k++){
    for(int u1 = 0;u1 < unum;u1++){
      double temp_u1 = preference[k][u1];
      for(int u2 = 0;u2 < unum;u2++){
        if (sg_adj[u1][u2]){
          temp_u1 += social_aware_weight[k][u1][u2];
        }
      }
      max_heaps[u1].push_back(temp_u1);
    }
  }
  // extract top-k items for each user
  for(int p = 0;p < pnum;p++){
    for(int u1 = 0;u1 < unum;u1++){
      make_heap(max_heaps[u1].begin(), max_heaps[u1].end());
      maximum[u1] += max_heaps[u1].front() ;
      max_heaps[u1].erase( max_heaps[u1].begin(), max_heaps[u1].begin()+1 );
    }
  }

  // calculate objective
  for(int p = 0;p < pnum;p++){
    for(int u1 = 0;u1 < unum;u1++){
      double utility_u1 = preference[ result[u1][p] ][u1];
      for(int u2 = 0;u2 < unum;u2++){
        if(sg_adj[u1][u2] && result[u1][p]==result[u2][p]){
          utility_u1 += social_aware_weight[ result[u1][p] ][u1][u2];
        }
      }
      objective[u1] += utility_u1;
    }
  }

  // cout report
  cout << "Regret repot:" << endl;
  cout << "\tobj\tmax\tregret" << endl;
  for(int u1 = 0;u1 < unum;u1++){
    cout << "U" << u1 << "\t" << objective[u1] << "\t" << maximum[u1] << "\t" << 1.-(objective[u1]/maximum[u1]) << endl;
  }
  
}

void print_summary(vector<vector<int> > result, vector<vector<double> > preference, vector<vector<vector<double> > > social_aware_weight, double* objective, double ***X, int unum, int inum, int pnum){
  cout<<endl;
  cout<<"Summary"<<endl;
  cout<<"=============================================================="<<endl;
  cout<<"Objective: "<<objective[0]<<endl;
  cout<<"Preference: "<<objective[1]<<endl;
  cout<<"Social: "<<objective[2]<<endl;
  cout<<"Unaligned: "<<objective[3]<<endl;

  vector<pair<int, int> > social_graph;
  Subgroup_output sout;
  sout.derive_social_graph(social_graph, social_aware_weight, unum, inum, 1);
  cout<<"=============================================================="<<endl;
  av_shared_people_per_item(result, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  sout.av_shared_item_per_friend_pair(social_graph, result, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  sout.av_density_in_group(result, social_graph, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  sout.regret(social_graph, result, preference, social_aware_weight, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
  result_p(result, unum, inum, pnum);
  //cout<<"=============================================================="<<endl;
  //av_pref_sc_per_item(result, preference, unum, inum, pnum);
  //cout<<"=============================================================="<<endl;
  //av_pref_sc_per_person(result, preference, unum, inum, pnum);
  cout<<"=============================================================="<<endl;
}

int main(int argc, char **argv) {
    MyUsage myUsage;
    int inum, unum, pnum;
    float max_X = 0.0;
    vector<vector<vector<double> > > social_aware_weight, X_fil;
    vector<vector<double> > preference;
    vector<vector<int> > result;
    vector<int> in_item;

    float lambda = 0.5;
    if (argc > 7){
      lambda = atof(argv[7]);
    }

    int group_size_ub = 10000;
    if (argc > 8){
      group_size_ub = atof(argv[8]);
    }

    read_dat(unum, inum, pnum, social_aware_weight, preference, lambda, argc, argv);
    double ***X = build_3D_array(unum, pnum, inum);
    max_X = solve(preference, social_aware_weight, X, X_fil, in_item, unum, inum, pnum, argc, argv);
    double  objective[4];
    for(int i=0;i<4;i++){
      objective[i] = 0;
    }
    //cout << "-----------------------------random algo." << endl;
    random_algorithm(result, X_fil, in_item, social_aware_weight, preference, objective, unum, inum, pnum, max_X, argv, group_size_ub);
    //cout << "-----------------------------myUsage" << endl;
    myUsage.report(1,1);
    //cout << "-----------------------------print summary" << endl;
    print_summary(result, preference, social_aware_weight, objective, X, unum, inum, pnum);
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

