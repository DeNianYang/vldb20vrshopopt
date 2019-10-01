all:
	g++ VRPR_randomized.cpp  -O2 -c
	g++ VRPR_randomized.o -O2 -lampl -L /usr/local/bin/amplapi/lib -o random.exe
	g++ VRPR_derandomized.cpp -O2 -fopenmp -c
	g++ VRPR_derandomized.o -O2 -lampl -L /usr/local/bin/amplapi/lib -fopenmp -o derandom.exe
	g++ data_generator.cpp -c
	g++ data_generator.o -o data_gen.exe
	g++ VRPR_ilp.cpp -c
	g++ VRPR_ilp.cpp -lampl -L /usr/local/bin/amplapi/lib -o ilp.exe
	g++ VRPR_lp.cpp -c
	g++ VRPR_lp.cpp -lampl -L /usr/local/bin/amplapi/lib -o lp.exe
	g++ data_type_generator.cpp -c
	g++ data_type_generator.o -o data_type_gen.exe
	g++ VRPR_ilp_user_study.cpp -c
	g++ VRPR_ilp_user_study.cpp -lampl -L /usr/local/bin/amplapi/lib -o ilp_user_study.exe
random: VRPR_randomized.o
	g++ VRPR_randomized.o -O2 -lampl -L /usr/local/bin/amplapi/lib -o random.exe
VRPR_randomnized.o:
	g++ VRPR_randomized.cpp -O2 -c
derandom: VRPR_derandomized.o
	g++ VRPR_derandomized.o -O2 -lampl -L /usr/local/bin/amplapi/lib -fopenmp -o derandom.exe
VRPR_derandomnized.o:
	g++ VRPR_derandomized.cpp -O2 -fopenmp -c
data: data_generator.o
	g++ data_generator.o -o data_gen.exe
data_generator.o:
	g++ data_generator.cpp -c
data_type: data_type_generator.o
	g++ data_type_generator.o -o data_type_gen.exe
data_type_generator.o:
	g++ data_type_generator.cpp -c
ilp: VRPR_ilp.o
	g++ VRPR_ilp.cpp -lampl -L /usr/local/bin/amplapi/lib -o ilp.exe
VRPR_ilp.o:
	g++ VRPR_ilp.cpp -c
lp: VRPR_lp.o
	g++ VRPR_lp.cpp -lampl -L /usr/local/bin/amplapi/lib -o lp.exe
VRPR_lp.o:
	g++ VRPR_lp.cpp -c
VRPR_ilp_user_study.o:
	g++ VRPR_ilp_user_study.cpp -c
ilp_user_study: VRPR_ilp_user_study.o
	g++ VRPR_ilp_user_study.cpp -lampl -L /usr/local/bin/amplapi/lib -o ilp_user_study.exe
clean:
	rm ./*.exe
	rm ./*.o
