#Unaligned Version of Problem
set User;
set Item;

param preference {i in Item, u in User};
param social_aware_weight {u in User, v in User, i in Item} ;
param pos;
param discount;

var x {u in User, p in 1..pos, i in Item} binary;
var y {u in User, v in User, p in 1..pos, i in Item} binary;
var z {u in User, v in User, i in Item} binary;

var obj_person;
var obj_social;
var obj_social_aligned;
var obj_social_unaligned;

maximize VRPR:
obj_person + obj_social;

subject to obj_social_def:
obj_social = obj_social_aligned + obj_social_unaligned;

subject to obj_person_definition:
obj_person = sum {u in User , i in Item} (preference[i, u] * (sum{p in 1..pos} x[u,p,i]));

subject to obj_social_aligned_definition:
obj_social_aligned = sum {u in User, v in User, i in Item} (social_aware_weight[u,v,i] * (sum {p in 1..pos} y[u,v,p,i]));

subject to obj_social_unaligned_definition:
obj_social_unaligned = sum {u in User, v in User, i in Item} (social_aware_weight[u,v,i] * discount * z[u,v,i]);

subject to one_item_each_position {p in 1..pos, u in User}:
sum{i in Item} x[u,p,i]==1;

subject to once_an_item {i in Item, u in User}:
sum{p in 1..pos} x[u,p,i] <= 1;

subject to x_larger_then_y_1 {u in User, v in User, p in 1..pos, i in Item}:
y[u,v,p,i] <= x[u,p,i];

subject to x_larger_then_y_2 {u in User, v in User, p in 1..pos, i in Item}:
y[u,v,p,i] <= x[v,p,i];

subject to x_larger_than_z_1 {u in User, v in User, i in Item}:
z[u,v,i] <= sum{p in 1..pos} x[u,p,i];

subject to x_larger_than_z_2 {u in User, v in User, i in Item}:
z[u,v,i] <= sum{p in 1..pos} x[v,p,i];

subject to if_y_then_no_z {u in User, v in User, i in Item}:
sum {p in 1..pos} y[u,v,p,i] + z[u,v,i] <= 1;

option presolve 0;
option gurobi_options 'outlev=1 timing=1';

