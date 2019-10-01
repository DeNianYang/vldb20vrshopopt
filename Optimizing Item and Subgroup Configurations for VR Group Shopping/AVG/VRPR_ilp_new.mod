#VRPR
set User;
set Item;

param preference {i in Item, u in User};
param social_aware_weight {u in User, v in User, i in Item} ;
param pos;

var x {u in User, p in 1..pos, i in Item} binary;
#var xe {u in User, i in Item}
#        =sum{p in 1..pos} x[u,p,i]; //suppressed to reduce ampl solveout time
var y {u in User, v in User, p in 1..pos, i in Item} binary;
#var ye {u in User, v in User, i in Item} 
#        = sum {p in 1..pos} y[u,v,p,i]; //suppressed to reduce ampl solveout time

var obj_person;
var obj_social;

maximize VRPR:
obj_person + obj_social;

subject to obj_person_definition:
obj_person = sum {u in User , i in Item} (preference[i, u] * (sum{p in 1..pos} x[u,p,i]));

subject to obj_social_definition:
obj_social = sum {u in User, v in User, i in Item} (social_aware_weight[u,v,i] * (sum {p in 1..pos} y[u,v,p,i]));

subject to one_item_each_position {p in 1..pos, u in User}:
sum{i in Item} x[u,p,i]==1;

subject to once_an_item {i in Item, u in User}:
sum{p in 1..pos} x[u,p,i] <= 1;

subject to x_larger_then_y_1 {u in User, v in User, p in 1..pos, i in Item}:
y[u,v,p,i] <= x[u,p,i];

subject to x_larger_then_y_2 {u in User, v in User, p in 1..pos, i in Item}:
y[u,v,p,i] <= x[v,p,i];

#subject to personal_preference {u in User}:///temporarily
#( sum{i in Item} (xe[u,i] * preference[i,u]))/( sum{i in Item} preference[i,u]) >= pos * Pj[u] / I;

option presolve 0;
option gurobi_options 'outlev=1 timing=1';

