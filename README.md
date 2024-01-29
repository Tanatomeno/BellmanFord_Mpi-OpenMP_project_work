## Ispiration:
https://github.com/qiansunn/BellmanFord.git

## Dependencies
* OpenMP
* MPI
* GNU Make 4.3
* Python 3.11.5 with libraries pandas lib, matplotlib lib, shutil lib.


## Easy testing instructions
1.  Use command "make" to compile Sequential and MPI+OpenMP versions.
2.  Then use command
3. Collab and mpi: 
  - If you want to test mpi on collab platfomr you may need to use the command: "!OMPI_ALLOW_RUN_AS_ROOT=1 OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1 make test_only_mpi_collab"
4. To obtain the average of different iterations of the tests (typically, tests are run for 2 times), use the Python script analize.py -The script also include the calculation of speedup and efficiency-
5. Similarly, to generate the graphs, use the Python script makeplots.py.
6. The outputs of the script can be viewed in the "results" folders for the analysis and "plots" folders for the generated plots.

## Notes for the tests
 * In the "files" folder, both input and output files are present. 
   * The input files are created in .txt and .bin (the bin is the default used inpunt)
   * The MPI parallelized algorithm generates a binary output file, unlike the sequential which produce a .txt file. 
   *  It is possible to make the MPI version also generate a .txt file by uncommenting #define binaryWriting in the main_parallel.
 * In case you also want to visualize the parent vector, which allows printing the paths, uncomment: the #define Parent from:
   * main_sequential.c; 
   * main_parallel.c; 
   * bellmanFord_parallel;
   * bellmanFord_sequential.
 * The paths printed by the sequential and parallel implementations are equivalent even if they are different. Both represent minimum paths.


## Files organization 
The most important files are the main ones:
 * Sets basic settings,
 * manages parallel input and output in the case of MPI
bellmanFord_sequential  
 * the sequential algorithm
bellmanFord_parallel:
 * the algorithm with MPI and OpenMP.

In the "main.c" file, all functions are included through "library.h" files.
 * The "main.c" files are located in 'src/'
 * The "function.c" files are all in 'src/referenceFolder/'
 * The "library.h" files are all in 'src/toInclude/referenceFolder/'
 * The ".o" files They are generated in the 'build/'
 * All the input and output files are generated in 'file/'
 * The folders...
    - 'information/', 
    - 'plots/',
    - 'tables/', 
    - 'result/' 
    * Are all divided into versions... 
        - 'mpi/', 
        * Each further divided by optimization level... 
            - 'opt0/', 
            - 'opt1/', 
            - 'opt2/', 
            - 'opt3/'

## The make file
The makefile has various commands:
 * _make_: Compile all programs in all optimization versions.
 * _make_ main_generator: Compile the matrix generation program
 * _make_ main_sequential: Compile the sequential version in all optimization versions
    * _make_ main_sequential0: Compile only optimization type 0
    * _make_ main_sequential1: Compile only optimization type 1
    * _make_ main_sequential2: Compile only optimization type 2
    * _make_ main_sequential3: Compile only optimization type 3
 * _make_ main_parallel: Compile only the MPI version in all optimization versions
    * _make_ main_sequential0: Compile only optimization type 0
    * _make_ main_sequential1: Compile only optimization type 1
    * _make_ main_sequential2: Compile only optimization type 2
    * _make_ main_sequential3: Compile only optimization type 3
 * _make_ clean: Clean the folders: 
    * _make_ clean_info: information/, plots/, results/, and tables/
    * _make_ clean_file: clean the file folder 
    * _make_ clean_exe: delete the files .o in src/build/ and .exe in src/
 * _make_ test: clean the file folder, clean information, plots, results and tables, run the matrix generation program and run the sequential and parallel programs in each configuration and optimization (they must have already been compiled).  
 * _make_ clear: it cleans /build, /plots, /result, /information, /file folders and delites .exe


# If the arguments are not specified, default values are used
If you want to test the algorithm with different inputs, it is possible to pass the appropriate arguments to the executables.
If you want to test the algorithms on alternative adjacency matrices, you can generate a binary file and pass the file path as the "filename" argument.

Every argument is optional:
- The filename automatically has the number of vertices and the extension appended to it.
- Number of vertices are 50 for default.
- Probability is 80% for default.
- Maximum weight is 20 for default.
- Minimum weight is 1 for default (you can use 0 <= values).
- Source vertex is 0 for default.
- Number of OpenMP threads is 0 for default.
- There is a default filename differente for evry main.

Arg:
 * ./main_generator0.exe (main_generator1.exe and main_generator2.exe and main_generator3.exe too)
   * [name.exe] -> Not used
   * [1] -> number of vertices
   * [2] -> seed
   * [3] -> Probability that there is an edge between two vertices
   * [4] -> Maximum weight a of the edges
   * [5] -> Minimum weight of the edges
   * [6] -> filename = Output file name to which the number of vertices and the extension will be added

 * ./main_sequenaial0.exe (main_sequenaial1.exe and main_sequenaial2.exe and main_sequenaial3.exe too)
   * [name.exe] -> The executable's name includes the optimization with which it was compiled  
   * [1] -> number of vertices
   * [3] -> vertexs source number
   * [4] -> filename = File from which to read the matrix
   * [5] -> filename2 = File to write the results to

 * mpiexec -n [process number] main_parallel0.exe (main_parallel1.exe and main_parallel2.exe and main_parallel3.exe too)
   * [name.exe] -> The executable's name includes the optimization with which it was compiled  
   * [1] -> number of vertices
   * [2] -> number of OpenMP threads (1 for default)
   * [3] -> vertexs source number
   * [4] -> filename = File from which to read the matrix
   * [5] -> filename2 = File to write the results to
