# Copyright (C) 2024 - All Rights Reserved
# This file is part of ProjectHPC.
# This program is free software: you can redistribute it and/or modify it under the terms of
# the GNU General Public License as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with ContestOMP.
# If not, see <https://www.gnu.org/licenses/gpl-3.0.html>.

CC = gcc
CC_PAR = mpicc

CFLAGS = -Wall
CFLAGS_PAR = $(CFLAGS) -fopenmp
OPTIMIZATION := -O0 -O1 -O2 -O3

COMPILE_EXE_PAR = $(CC_PAR) $(CFLAGS_PAR) $^ -o $@.exe
COMPILE_EXE = gcc -Wall $^ -o $@.exe 

COMPILE_O_PAR = $(CC_PAR) $(CFLAGS_PAR) -c $< -o $@
COMPILE_O = gcc -Wall -c $< -o $@

SRC_DIR = src
OBJ_DIR = build
INC = $(SRC_DIR)/toInclude
INC_BF = $(INC)/BellmanFord
INC_GEN = $(INC)/Generator
INC_IO = $(INC)/IO

SRC_BF = $(SRC_DIR)/BellmanFord
SRC_GEN = $(SRC_DIR)/Generator
SRC_IO = $(SRC_DIR)/IO


BF_S_0 = $(OBJ_DIR)/print_bellmanFord_sequential_txt0.o $(OBJ_DIR)/print_bellmanFord_sequential0.o $(OBJ_DIR)/bellmanFord_sequential0.o
BF_S_1 = $(OBJ_DIR)/print_bellmanFord_sequential_txt1.o $(OBJ_DIR)/print_bellmanFord_sequential1.o $(OBJ_DIR)/bellmanFord_sequential1.o
BF_S_2 = $(OBJ_DIR)/print_bellmanFord_sequential_txt2.o $(OBJ_DIR)/print_bellmanFord_sequential2.o $(OBJ_DIR)/bellmanFord_sequential2.o
BF_S_3 = $(OBJ_DIR)/print_bellmanFord_sequential_txt3.o $(OBJ_DIR)/print_bellmanFord_sequential3.o $(OBJ_DIR)/bellmanFord_sequential3.o

BF_P_0 = $(OBJ_DIR)/print_bellmanFord_sequential_txt0.o $(OBJ_DIR)/print_bellmanFord_sequential0.o $(OBJ_DIR)/bellmanFord_parallel0.o
BF_P_1 = $(OBJ_DIR)/print_bellmanFord_sequential_txt1.o $(OBJ_DIR)/print_bellmanFord_sequential1.o $(OBJ_DIR)/bellmanFord_parallel1.o
BF_P_2 = $(OBJ_DIR)/print_bellmanFord_sequential_txt2.o $(OBJ_DIR)/print_bellmanFord_sequential2.o $(OBJ_DIR)/bellmanFord_parallel2.o
BF_P_3 = $(OBJ_DIR)/print_bellmanFord_sequential_txt3.o $(OBJ_DIR)/print_bellmanFord_sequential3.o $(OBJ_DIR)/bellmanFord_parallel3.o

GEN = $(OBJ_DIR)/adjacencyMatrixGen0.o $(OBJ_DIR)/adjacencyMatrixGen1D0.o

IO_0 = $(OBJ_DIR)/printGraph0.o $(OBJ_DIR)/readGraphFromFile0.o $(OBJ_DIR)/printGraphToFileAsTxt0.o $(OBJ_DIR)/printGraphToFile0.o $(OBJ_DIR)/readGraphFromFile1D0.o $(OBJ_DIR)/printGraphToFileAsTxt1D0.o $(OBJ_DIR)/printGraphToFile1D0.o $(OBJ_DIR)/printGraph1D0.o
IO_1 = $(OBJ_DIR)/printGraph1.o $(OBJ_DIR)/readGraphFromFile1.o $(OBJ_DIR)/printGraphToFileAsTxt1.o $(OBJ_DIR)/printGraphToFile1.o $(OBJ_DIR)/readGraphFromFile1D1.o $(OBJ_DIR)/printGraphToFileAsTxt1D1.o $(OBJ_DIR)/printGraphToFile1D1.o $(OBJ_DIR)/printGraph1D1.o
IO_2 = $(OBJ_DIR)/printGraph2.o $(OBJ_DIR)/readGraphFromFile2.o $(OBJ_DIR)/printGraphToFileAsTxt2.o $(OBJ_DIR)/printGraphToFile2.o $(OBJ_DIR)/readGraphFromFile1D2.o $(OBJ_DIR)/printGraphToFileAsTxt1D2.o $(OBJ_DIR)/printGraphToFile1D2.o $(OBJ_DIR)/printGraph1D2.o
IO_3 = $(OBJ_DIR)/printGraph3.o $(OBJ_DIR)/readGraphFromFile3.o $(OBJ_DIR)/printGraphToFileAsTxt3.o $(OBJ_DIR)/printGraphToFile3.o $(OBJ_DIR)/readGraphFromFile1D3.o $(OBJ_DIR)/printGraphToFileAsTxt1D3.o $(OBJ_DIR)/printGraphToFile1D3.o $(OBJ_DIR)/printGraph1D3.o



all: dir_creation clean_exe main_generator main_sequential main_parallel


firstdir := Informations Results Plots 
seconddir := OMP_MPI
thirddir := opt0 opt1 opt2 opt3
dir_creation: 
	mkdir file
	mkdir build
	for fdir in $(firstdir); do \
		mkdir -p $$fdir ; \
		for sdir in $(seconddir); do \
			mkdir -p $$fdir/$$sdir ; \
			for tdir in $(thirddir); do \
				mkdir -p $$fdir/$$sdir/$$tdir ; \
			done; \
		done; \
	done



clean: 
	rm -rf file
	rm -rf build
	for fdir in $(firstdir); do \
		rm -rf $$fdir ; \
	done

##
main_parallel: main_parallel0 main_parallel1 main_parallel2 main_parallel3

main_parallel0: $(OBJ_DIR)/main_parallel0.o $(BF_P_0)  $(IO_0) $(OBJ_DIR)/printCSV0.o
	$(COMPILE_EXE_PAR) -O0

main_parallel1: $(OBJ_DIR)/main_parallel1.o $(BF_P_1)  $(IO_1) $(OBJ_DIR)/printCSV1.o
	$(COMPILE_EXE_PAR) -O1

main_parallel2: $(OBJ_DIR)/main_parallel2.o $(BF_P_2)  $(IO_2) $(OBJ_DIR)/printCSV2.o
	$(COMPILE_EXE_PAR) -O2

main_parallel3: $(OBJ_DIR)/main_parallel3.o $(BF_P_3)  $(IO_3) $(OBJ_DIR)/printCSV3.o
	$(COMPILE_EXE_PAR) -O3		

$(OBJ_DIR)/main_parallel0.o: $(SRC_DIR)/main_parallel.c  
	$(COMPILE_O_PAR) -O0

$(OBJ_DIR)/main_parallel1.o: $(SRC_DIR)/main_parallel.c  
	$(COMPILE_O_PAR) -O1

$(OBJ_DIR)/main_parallel2.o: $(SRC_DIR)/main_parallel.c  
	$(COMPILE_O_PAR) -O2

$(OBJ_DIR)/main_parallel3.o: $(SRC_DIR)/main_parallel.c  
	$(COMPILE_O_PAR) -O3








##
main_generator:  $(OBJ_DIR)/main_generator.o $(IO_0) $(GEN)
	$(COMPILE_EXE) 


$(OBJ_DIR)/main_generator.o: $(SRC_DIR)/main_generator.c  
	$(COMPILE_O) 





##
main_sequential: main_sequential0 main_sequential1 main_sequential2 main_sequential3

main_sequential0:  $(OBJ_DIR)/main_sequential0.o $(BF_S_0) $(IO_0) $(OBJ_DIR)/printCSV0.o 
	$(COMPILE_EXE) -O0

main_sequential1:  $(OBJ_DIR)/main_sequential1.o $(BF_S_1) $(IO_1) $(OBJ_DIR)/printCSV1.o
	$(COMPILE_EXE) -O1

main_sequential2:  $(OBJ_DIR)/main_sequential2.o $(BF_S_2) $(IO_2) $(OBJ_DIR)/printCSV2.o
	$(COMPILE_EXE) -O2

main_sequential3:  $(OBJ_DIR)/main_sequential3.o $(BF_S_3) $(IO_3) $(OBJ_DIR)/printCSV3.o
	$(COMPILE_EXE) -O3

$(OBJ_DIR)/main_sequential0.o: $(SRC_DIR)/main_sequential.c 
	$(COMPILE_O) -O0

$(OBJ_DIR)/main_sequential1.o: $(SRC_DIR)/main_sequential.c 
	$(COMPILE_O) -O1

$(OBJ_DIR)/main_sequential2.o: $(SRC_DIR)/main_sequential.c 
	$(COMPILE_O) -O2

$(OBJ_DIR)/main_sequential3.o: $(SRC_DIR)/main_sequential.c 
	$(COMPILE_O) -O3


#printCSV
$(OBJ_DIR)/printCSV0.o: $(SRC_DIR)/printCSV.c  
	$(COMPILE_O) -O0

$(OBJ_DIR)/printCSV1.o: $(SRC_DIR)/printCSV.c  
	$(COMPILE_O) -O1

$(OBJ_DIR)/printCSV2.o: $(SRC_DIR)/printCSV.c  
	$(COMPILE_O) -O2

$(OBJ_DIR)/printCSV3.o: $(SRC_DIR)/printCSV.c  
	$(COMPILE_O) -O3


#GEN.o
$(OBJ_DIR)/adjacencyMatrixGen0.o: $(SRC_GEN)/adjacencyMatrixGen.c $(INC_GEN)/adjacencyMatrixGen.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/adjacencyMatrixGen1D0.o: $(SRC_GEN)/adjacencyMatrixGen1D.c $(INC_GEN)/adjacencyMatrixGen1D.h
	$(COMPILE_O) -O0







#BF_S.o and BF_P.o
### -O0

$(OBJ_DIR)/bellmanFord_parallel0.o: $(SRC_BF)/bellmanFord_parallel.c $(INC_BF)/bellmanFord_parallel.h
	$(COMPILE_O_PAR) -O0

$(OBJ_DIR)/print_bellmanFord_sequential_txt0.o: $(SRC_BF)/print_bellmanFord_sequential_txt.c $(INC_BF)/print_bellmanFord_sequential_txt.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/print_bellmanFord_sequential0.o: $(SRC_BF)/print_bellmanFord_sequential.c $(INC_BF)/print_bellmanFord_sequential.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/bellmanFord_sequential0.o: $(SRC_BF)/bellmanFord_sequential.c $(INC_BF)/bellmanFord_sequential.h
	$(COMPILE_O) -O0

### -O1

$(OBJ_DIR)/bellmanFord_parallel1.o: $(SRC_BF)/bellmanFord_parallel.c $(INC_BF)/bellmanFord_parallel.h
	$(COMPILE_O_PAR) -O1

$(OBJ_DIR)/print_bellmanFord_sequential_txt1.o: $(SRC_BF)/print_bellmanFord_sequential_txt.c $(INC_BF)/print_bellmanFord_sequential_txt.h
	$(COMPILE_O) -O1

$(OBJ_DIR)/print_bellmanFord_sequential1.o: $(SRC_BF)/print_bellmanFord_sequential.c $(INC_BF)/print_bellmanFord_sequential.h
	$(COMPILE_O) -O1

$(OBJ_DIR)/bellmanFord_sequential1.o: $(SRC_BF)/bellmanFord_sequential.c $(INC_BF)/bellmanFord_sequential.h
	$(COMPILE_O) -O1

### -O2

$(OBJ_DIR)/bellmanFord_parallel2.o: $(SRC_BF)/bellmanFord_parallel.c $(INC_BF)/bellmanFord_parallel.h
	$(COMPILE_O_PAR) -O2

$(OBJ_DIR)/print_bellmanFord_sequential_txt2.o: $(SRC_BF)/print_bellmanFord_sequential_txt.c $(INC_BF)/print_bellmanFord_sequential_txt.h
	$(COMPILE_O) -O2

$(OBJ_DIR)/print_bellmanFord_sequential2.o: $(SRC_BF)/print_bellmanFord_sequential.c $(INC_BF)/print_bellmanFord_sequential.h
	$(COMPILE_O) -O2

$(OBJ_DIR)/bellmanFord_sequential2.o: $(SRC_BF)/bellmanFord_sequential.c $(INC_BF)/bellmanFord_sequential.h
	$(COMPILE_O) -O2

### -O3

$(OBJ_DIR)/bellmanFord_parallel3.o: $(SRC_BF)/bellmanFord_parallel.c $(INC_BF)/bellmanFord_parallel.h
	$(COMPILE_O_PAR) -O3

$(OBJ_DIR)/print_bellmanFord_sequential_txt3.o: $(SRC_BF)/print_bellmanFord_sequential_txt.c $(INC_BF)/print_bellmanFord_sequential_txt.h
	$(COMPILE_O) -O3

$(OBJ_DIR)/print_bellmanFord_sequential3.o: $(SRC_BF)/print_bellmanFord_sequential.c $(INC_BF)/print_bellmanFord_sequential.h
	$(COMPILE_O) -O3

$(OBJ_DIR)/bellmanFord_sequential3.o: $(SRC_BF)/bellmanFord_sequential.c $(INC_BF)/bellmanFord_sequential.h
	$(COMPILE_O) -O3








#IO.o
##1D

### -O0
$(OBJ_DIR)/printGraphToFile1D0.o: $(SRC_IO)/printGraphToFile1D.c $(INC_IO)/printGraphToFile1D.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/printGraphToFileAsTxt1D0.o: $(SRC_IO)/printGraphToFileAsTxt1D.c $(INC_IO)/printGraphToFileAsTxt1D.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/readGraphFromFile1D0.o: $(SRC_IO)/readGraphFromFile1D.c $(INC_IO)/readGraphFromFile1D.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/printGraph1D0.o: $(SRC_IO)/printGraph1D.c $(INC_IO)/printGraph1D.h
	$(COMPILE_O) -O0

### -O1
$(OBJ_DIR)/printGraphToFile1D1.o: $(SRC_IO)/printGraphToFile1D.c $(INC_IO)/printGraphToFile1D.h
	$(COMPILE_O) -O1

$(OBJ_DIR)/printGraphToFileAsTxt1D1.o: $(SRC_IO)/printGraphToFileAsTxt1D.c $(INC_IO)/printGraphToFileAsTxt1D.h
	$(COMPILE_O) -O1

$(OBJ_DIR)/readGraphFromFile1D1.o: $(SRC_IO)/readGraphFromFile1D.c $(INC_IO)/readGraphFromFile1D.h
	$(COMPILE_O) -O1

$(OBJ_DIR)/printGraph1D1.o: $(SRC_IO)/printGraph1D.c $(INC_IO)/printGraph1D.h
	$(COMPILE_O) -O1

### -O2
$(OBJ_DIR)/printGraphToFile1D2.o: $(SRC_IO)/printGraphToFile1D.c $(INC_IO)/printGraphToFile1D.h
	$(COMPILE_O) -O2

$(OBJ_DIR)/printGraphToFileAsTxt1D2.o: $(SRC_IO)/printGraphToFileAsTxt1D.c $(INC_IO)/printGraphToFileAsTxt1D.h
	$(COMPILE_O) -O2

$(OBJ_DIR)/readGraphFromFile1D2.o: $(SRC_IO)/readGraphFromFile1D.c $(INC_IO)/readGraphFromFile1D.h
	$(COMPILE_O) -O2

$(OBJ_DIR)/printGraph1D2.o: $(SRC_IO)/printGraph1D.c $(INC_IO)/printGraph1D.h
	$(COMPILE_O) -O2

### -O3
$(OBJ_DIR)/printGraphToFile1D3.o: $(SRC_IO)/printGraphToFile1D.c $(INC_IO)/printGraphToFile1D.h
	$(COMPILE_O) -O3

$(OBJ_DIR)/printGraphToFileAsTxt1D3.o: $(SRC_IO)/printGraphToFileAsTxt1D.c $(INC_IO)/printGraphToFileAsTxt1D.h
	$(COMPILE_O) -O3

$(OBJ_DIR)/readGraphFromFile1D3.o: $(SRC_IO)/readGraphFromFile1D.c $(INC_IO)/readGraphFromFile1D.h
	$(COMPILE_O) -O3

$(OBJ_DIR)/printGraph1D3.o: $(SRC_IO)/printGraph1D.c $(INC_IO)/printGraph1D.h
	$(COMPILE_O) -O3



##2D

### -O0
$(OBJ_DIR)/printGraphToFile0.o: $(SRC_IO)/printGraphToFile.c $(INC_IO)/printGraphToFile.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/printGraphToFileAsTxt0.o: $(SRC_IO)/printGraphToFileAsTxt.c $(INC_IO)/printGraphToFileAsTxt.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/readGraphFromFile0.o: $(SRC_IO)/readGraphFromFile.c $(INC_IO)/readGraphFromFile.h
	$(COMPILE_O) -O0

$(OBJ_DIR)/printGraph0.o: $(SRC_IO)/printGraph.c $(INC_IO)/printGraph.h
	$(COMPILE_O) -O0
#### -O1
$(OBJ_DIR)/printGraphToFile1.o: $(SRC_IO)/printGraphToFile.c $(INC_IO)/printGraphToFile.h
	$(COMPILE_O) -O1

$(OBJ_DIR)/printGraphToFileAsTxt1.o: $(SRC_IO)/printGraphToFileAsTxt.c $(INC_IO)/printGraphToFileAsTxt.h
	$(COMPILE_O) -O1

$(OBJ_DIR)/readGraphFromFile1.o: $(SRC_IO)/readGraphFromFile.c $(INC_IO)/readGraphFromFile.h
	$(COMPILE_O) -O1

$(OBJ_DIR)/printGraph1.o: $(SRC_IO)/printGraph.c $(INC_IO)/printGraph.h
	$(COMPILE_O) -O1
#### -O2
$(OBJ_DIR)/printGraphToFile2.o: $(SRC_IO)/printGraphToFile.c $(INC_IO)/printGraphToFile.h
	$(COMPILE_O) -O2

$(OBJ_DIR)/printGraphToFileAsTxt2.o: $(SRC_IO)/printGraphToFileAsTxt.c $(INC_IO)/printGraphToFileAsTxt.h
	$(COMPILE_O) -O2

$(OBJ_DIR)/readGraphFromFile2.o: $(SRC_IO)/readGraphFromFile.c $(INC_IO)/readGraphFromFile.h
	$(COMPILE_O) -O2

$(OBJ_DIR)/printGraph2.o: $(SRC_IO)/printGraph.c $(INC_IO)/printGraph.h
	$(COMPILE_O) -O2
##### -O3
$(OBJ_DIR)/printGraphToFile3.o: $(SRC_IO)/printGraphToFile.c $(INC_IO)/printGraphToFile.h
	$(COMPILE_O) -O3

$(OBJ_DIR)/printGraphToFileAsTxt3.o: $(SRC_IO)/printGraphToFileAsTxt.c $(INC_IO)/printGraphToFileAsTxt.h
	$(COMPILE_O) -O3

$(OBJ_DIR)/readGraphFromFile3.o: $(SRC_IO)/readGraphFromFile.c $(INC_IO)/readGraphFromFile.h
	$(COMPILE_O) -O3

$(OBJ_DIR)/printGraph3.o: $(SRC_IO)/printGraph.c $(INC_IO)/printGraph.h
	$(COMPILE_O) -O3







clean2: clean_info clean_file clean_exe

clean_exe:
	rm -f $(OBJ_DIR)/*.o
	rm -f main_*


clean_file:	
	rm -f file/*

clean_info:
	rm -rf Informations/*
	rm -rf Results/*
	rm -rf Plots/*




################### TEST
n_nodes := 4000 8000 12000
omp_threads := 1 2 3 4 5 6
mpi_process := 1 2 3 4 5 6
opm_tipe := 0 1 2 3
iterations = 1 2




test: test80

test80: generate80 test_sequential test_mpi
 
test_only_mpi80: generate80 test_sequential test_mpi

test_only_mpi_collab80: generate80 test_sequential test_mpi_collab



####

test20: generate20 test_sequential test_mpi
 
test_only_mpi: generate20 test_sequential test_mpi

test_only_mpi_collab: generate20 test_sequential test_mpi_collab


#generetion file 
generate20: clean_file
	for node in $(n_nodes); do \
		echo "generating a graph with V=$$node node..."; \
		./main_generator.exe $$node 1 0.2; \
	done

generate80: clean_file
	for node in $(n_nodes); do \
		echo "generating a graph with V=$$node node..."; \
		./main_generator.exe $$node 1 0.8; \
	done

#test sequential
test_sequential: 
	for it in $(iterations); do \
		for node in $(n_nodes); do \
			for opm in $(opm_tipe); do \
				echo "interation:$$it doing sequential$$opm V=$$node..."; \
				./main_sequential$$opm.exe $$node; \
			done; \
		done; \
	done

#test OpenMP MPI
test_mpi: 
	for it in $(iterations); do \
		for node in $(n_nodes); do \
			for opm in $(opm_tipe); do \
				for n_threads in $(omp_threads); do \
					for mpi in $(mpi_process); do \
						echo "interation:$$it doing parallel$$opm V=$$node threadOpenMP=$$n_threads MPIProcess=$$mpi..."; \
						mpiexec -n $$mpi ./main_parallel$$opm.exe $$node $$n_threads; \
					done; \
				done; \
			done; \
		done; \
	done



#test OpenMP MPI
test_mpi_collab: 
	for it in $(iterations); do \
		for node in $(n_nodes); do \
			for opm in $(opm_tipe); do \
				for n_threads in $(omp_threads); do \
					for mpi in $(mpi_process); do \
						echo "interation:$$it doing parallel$$opm V=$$node threadOpenMP=$$n_threads MPIProcess=$$mpi..."; \
						mpiexec -oversubscribe -n $$mpi ./main_parallel$$opm.exe $$node $$n_threads; \
					done; \
				done; \
			done; \
		done; \
	done

	


analize:
	python analize.py
