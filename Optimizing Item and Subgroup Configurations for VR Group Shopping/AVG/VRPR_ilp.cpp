#include "ampl/ampl.h"
#include <iostream>

int main(int argc, char **argv) {
  ampl::AMPL ampl;

  ampl.setOption("solver", "gurobi_ampl");

  // Read the model and data files.
  std::string modelDirectory = "VRPR_ilp_new.mod";
  std::string dataDirectory = argv[1];
  ampl.read(modelDirectory);
  ampl.readData(dataDirectory);

  //options
  std::string threads = "1";
  std::string timelim = "100000";

  if (argc > 2){
    threads = argv[2];
  }
  if (argc > 3){
    timelim = argv[3];
  }

  std::string gurobi_option_string = "option gurobi_options 'outlev=1 timing=1 threads=";
  gurobi_option_string += threads;
  gurobi_option_string += " timelim=";
  gurobi_option_string += timelim;
  gurobi_option_string += "';";
  ampl.eval(gurobi_option_string);
    
     // Solve
  ampl.solve();
  std::cout<<"ILP Objective Maximize: "<<ampl.getObjective("VRPR").value()<<"\n";
  std::cout<<"Personalized Objective: "<<ampl.getVariable("obj_person").value()<<"\n";
  std::cout<<"Social Objective: "<<ampl.getVariable("obj_social").value()<<"\n";
}
