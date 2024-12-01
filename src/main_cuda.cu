/*
 * Course: High Performance Computing 2023/2024
 *
 * Lecturer: Francesco Moscato	fmoscato@unisa.it
 *
 * Author:
 * Del Gaudio Nunzio		0612705044		n.delgaudio5@studenti.unisa.it
 *
 * Copyright (C) 2024 - All Rights Reserved
 *
 * This file is part of ProjectHPC.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with ContestOMP.
 * If not, see <https://www.gnu.org/licenses/gpl-3.0.html>.
 */




/*-----------------------------------------------------------------------------------------------------------------
//NOTE: Due to linking issues, all functions previously declared in the project have been redefined in this file.
//These functions have not been commented.
/-----------------------------------------------------------------------------------------------------------------*/


#include <cuda.h>
#include <cuda_runtime.h>
#include <omp.h>
#include <ctype.h>
#include <stdio.h> //printf and fprint
#include <stdlib.h> //malloc
#include <string.h> //strcpy
#include <sys/time.h> //for csv
#include <limits.h> //for INT_MAX
#include <algorithm>

#define ABSENTE_EDGE INT_MAX
#define INF INT_MAX
#define DEFAULT_FILENAME "file/RandomMatrix"
#define DEFAULT_FILENAME1D "file/RandomMatrix"
#define DEFAULT_V 50 // Default Vertexs number
#define DEFAULT_EDGE_PROBABILITY 0.8
#define DEFAULT_WEIGHT_MAX 20
#define DEFAULT_WEIGHT_MIN 1   //In order to reduce the probability of negative cycle
#define DEFAULT_SOURCE 0
#define DEFAULT_FILENAME_BellmanFord "file/Bellmanford"
#define DEFAULT_FILENAME_BellmanFord_Parallel "file/Bellmanford_Parallel"
#define DEFAULT_FILENAME_BellmanFord_Cuda "file/Bellmanford_Cuda"



//In order to avoid having to rewrite an error check for each cuda function
#define CUDA_CHECK(call) {\
    cudaError_t cudaStatus = call;\
    if (cudaStatus != cudaSuccess) {\
        fprintf(stderr, "CUDA_ERROR: %s in file %s at line %d\nerror number:", cudaGetErrorString(cudaStatus), __FILE__, __LINE__ );\
        exit(EXIT_FAILURE);\
    }\
}


/*
for Compute Capability 7.5 
With the assumption of having enough registers for 
all threads in a SM (which is true).
Because each Streaming Multiprocessor (SM)
can have a maximum of 16 blocks and 1024 threads,
and each block can have a maximum of 1024 threads, 
I set the number of threads per block to 64 to occupy 
the maximum number of blocks and threads per SM.
64*16=1024
*/
#define MAX_DIMBLOCK 64


int print_bellmanFord_sequential(int , int * , int * , int );
void printPath(int* , int , int );
int print_bellmanFord_sequential_txt(char* , int , int* , int* , int );
void printPath_txt(FILE*, int* , int , int );
int print_bellmanFord_negativeCicle(char*);
char * filenameInizialzie(char[], int );
int adjacencyMatrixGen1D(int, int , int  , float , unsigned int , int *);
int printGraph1D(int * , int );
int bellmanFord_cuda(int *  , int ,  int , int *, int* , dim3  , dim3 , int );
int readGraphFromFile1D(int *, int , char *);
int printCSV(const char [],int , int , int , int , double , double , double , double );
double timeval_to_microseconds(struct timeval *, struct timeval *);



#define TESTING
#define SHARED


//We use a structure to work with edges so that we can pass the edges to the kernel

/// @brief structure used to define an arch
typedef struct {
    int start;
    int end;
    int weight;
} Edge;


/// @brief variable to indicate to the main that the distance vector has been updated during the iteration
__device__ int Updated;

/// @brief Kernel without the use of shared memory
/// @param edgeNumber Number of edges (excluding infinite weight)
/// @param device_EdgeArray Pointer to the edges
/// @param device_Distances Pointer to the distances
/// @return none
__global__ void bellmanFord_kernel_noShared ( int edgeNumber, Edge * device_EdgeArray, int * device_Distances) {
    
    int oldval, newval;
    unsigned int global_Id = blockIdx.x * blockDim.x + threadIdx.x;

    Updated=0;

    const unsigned int  i = device_EdgeArray[global_Id].start;
    const unsigned int  j = device_EdgeArray[global_Id].end;

    if(device_Distances[i]!=INF && global_Id < edgeNumber){
        if( device_Distances[j]> device_Distances[i] + device_EdgeArray[global_Id].weight ) {
                
            Updated=1;

            do { //for device_Distances
                oldval = device_Distances[j];
                newval=  device_Distances[i] + device_EdgeArray[global_Id].weight;
                newval=fminf(oldval, newval);

            } while( (atomicCAS((int*)&device_Distances[j], oldval, newval) != newval) );
        }
    }
}



/// @brief Kernel with the use of shared memory for the edge
/// @param edgeNumber same
/// @param device_EdgeArray  same
/// @param device_Distances same
/// @return none
__global__ void bellmanFord_kernel ( int edgeNumber, Edge * device_EdgeArray, int * device_Distances) {
    
    int oldval, newval;
    unsigned int global_Id = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int local_Id = threadIdx.x;

    Updated=0;

    //In this case, shared memory is used as a buffer 
    //and not to share data between threads within the same block
    __shared__ Edge buffer[MAX_DIMBLOCK];


    if(global_Id < edgeNumber) {
        buffer[local_Id] = device_EdgeArray[global_Id];
    }
    

    __syncthreads();

    const unsigned int  i = buffer[local_Id].start;
    const unsigned int  j = buffer[local_Id].end;


    if(device_Distances[i]!=INF && global_Id < edgeNumber){
        if( device_Distances[j]> device_Distances[i] + buffer[local_Id].weight ) {
               
            Updated=1;

            do { //for device_Distances
                oldval = device_Distances[j];
                newval= device_Distances[i] + buffer[local_Id].weight;
                newval=fminf(oldval, newval);
            } while( (atomicCAS((int*)&device_Distances[j], oldval, newval) != newval) );
        }
    }
}
 

/// @brief "Bellman-Ford function that invokes the kernel
/// @param host_Graph Adjacency matrix
/// @param V Number of vertices
/// @param src Starting vertex
/// @param host_Distances Vector of distances
/// @param blocksize Number of threads in a block
/// @param totalThread Total threads executed
/// @param tipe 0 = shared memory and L1 < sh; 1 = no shared memory and  L1 > sh; 2 = shared memory and L1 >sh
/// @return If errors have occurred
int bellmanFord_cuda(int * host_Graph , int V,  int src, int *host_Distances, int blocksize, int*  totalThread, int tipe) {

    cudaGetLastError();

    int host_hasNegativeCycle = 0;
    int i,j;
    Edge* device_EdgeArray;
    int *device_Distances;
    Edge* host_EdgeArray;

    //OpenMP!
    int edgeNumber = 0; 
    #pragma omp parallel for default(none) shared(host_Graph,V) reduction(+:edgeNumber) private (i,j)
    for( i=0; i<V; i++){
        for( j=0; j<V; j++){ 
            if(host_Graph[i*V+j]!=INF){
                edgeNumber++; //We count the number of edges actually present in the adjacency matrix
            }
        }
    }


    int size_Distances = V * sizeof(int);
    int size_Graph = edgeNumber * sizeof(Edge);
    
    //Allocation of the edges vector
    host_EdgeArray = (Edge*)malloc(edgeNumber* sizeof(Edge));
    if (host_EdgeArray == NULL) {
        fprintf(stderr, "edge malloc error\n");
        return -1; 
    }


    //Populating the edges vector
    int x = 0;
    for( i=0; i<V; i++){
        for( j=0; j<V; j++){ 
            if(host_Graph[i*V+j]!=INF){
                host_EdgeArray[x].weight=host_Graph[i*V+j];
                host_EdgeArray[x].start=i;
                host_EdgeArray[x].end=j;   
                x++;            
            }
        }
    }



    //OpenMP!
    #pragma omp parallel for default(none) shared(host_Distances,V) private(i)
    for(i=0; i<V; i++) {
        host_Distances[i] = INF;
    }
    host_Distances[src] = 0;

    

    dim3 threadForBlocks(blocksize);
    dim3 blocksForGrid(((edgeNumber-1) / blocksize)+1); //we allocate a sufficient number of blocks so that each thread can work with an edge
    *totalThread = (threadForBlocks.x)*(blocksForGrid.x);


    //CUDA! maclloc
    CUDA_CHECK( cudaMalloc((void**)&device_Distances, size_Distances) );
    CUDA_CHECK( cudaMalloc((void**)&device_EdgeArray, size_Graph) );
    
    //copy
    CUDA_CHECK( cudaMemcpy(device_EdgeArray, host_EdgeArray, size_Graph, cudaMemcpyHostToDevice) );
    CUDA_CHECK( cudaMemcpy(device_Distances, host_Distances, size_Distances, cudaMemcpyHostToDevice) );

    
    int iterNumber=0;
    int host_updated=0;

    if (tipe==0){ //ShMem > L1; yes shared
        cudaFuncSetCacheConfig(bellmanFord_kernel, cudaFuncCachePreferShared); 
        for(int i=0; i<V-1; i++){//V-1 kernell calling
            bellmanFord_kernel <<< blocksForGrid, threadForBlocks >>> (edgeNumber, device_EdgeArray, device_Distances);
            
            CUDA_CHECK( cudaDeviceSynchronize() );
            iterNumber++;
            cudaMemcpyFromSymbol(&host_updated, Updated, sizeof(int));
            if(host_updated==0){
                break;
            }

        }
    }else if(tipe==1){ //ShMem < L1; No shared
        cudaFuncSetCacheConfig(bellmanFord_kernel, cudaFuncCachePreferL1); 
        for(int i=0; i<V-1; i++){//V-1 kernell calling
            bellmanFord_kernel_noShared <<< blocksForGrid, threadForBlocks >>> (edgeNumber, device_EdgeArray, device_Distances);
            
            CUDA_CHECK( cudaDeviceSynchronize() );
            iterNumber++;
            cudaMemcpyFromSymbol(&host_updated, Updated, sizeof(int));
            if(host_updated==0){
                break;
            }
        }
    }else if(tipe==2){ //ShMem < L1; yes shared
        cudaFuncSetCacheConfig(bellmanFord_kernel, cudaFuncCachePreferL1); 
        for(int i=0; i<V-1; i++){//V-1 kernell calling
           bellmanFord_kernel <<< blocksForGrid, threadForBlocks >>> (edgeNumber, device_EdgeArray, device_Distances);
            
            CUDA_CHECK( cudaDeviceSynchronize() );
            iterNumber++;
            cudaMemcpyFromSymbol(&host_updated, Updated, sizeof(int));
            if(host_updated==0){
                break;
            }
        }
    }else{
        fprintf(stderr, "cuda tipe not allowed\n");
        return -1;
    }
    
    

    //Similarly, for the distance search, I opted not to use the define for improved code readability.
    if(iterNumber==V-1){
        host_hasNegativeCycle = 1;
        if (tipe==0){ 
            cudaFuncSetCacheConfig(bellmanFord_kernel, cudaFuncCachePreferShared); 
                bellmanFord_kernel <<< blocksForGrid, threadForBlocks >>> (edgeNumber, device_EdgeArray, device_Distances);
                
                CUDA_CHECK( cudaDeviceSynchronize() );
                iterNumber++;
                cudaMemcpyFromSymbol(&host_updated, Updated, sizeof(int));
                if(host_updated==0){
                    host_hasNegativeCycle=0;
                }
        }else if(tipe==1){ 
            cudaFuncSetCacheConfig(bellmanFord_kernel, cudaFuncCachePreferL1); 
                bellmanFord_kernel_noShared <<< blocksForGrid, threadForBlocks >>> (edgeNumber, device_EdgeArray, device_Distances);
                
                CUDA_CHECK( cudaDeviceSynchronize() );
                iterNumber++;
                cudaMemcpyFromSymbol(&host_updated, Updated, sizeof(int));
                if(host_updated==0){
                    host_hasNegativeCycle=0;
                }
        }else if(tipe==2){ //ShMem < L1; yes shared
            cudaFuncSetCacheConfig(bellmanFord_kernel, cudaFuncCachePreferL1); 
            bellmanFord_kernel <<< blocksForGrid, threadForBlocks >>> (edgeNumber, device_EdgeArray, device_Distances);
                
                CUDA_CHECK( cudaDeviceSynchronize() );
                iterNumber++;
                cudaMemcpyFromSymbol(&host_updated, Updated, sizeof(int));
                if(host_updated==0){
                    host_hasNegativeCycle=0;
                }
        }
    }



    // copy-back of the result
    if(host_hasNegativeCycle==0)
    CUDA_CHECK( cudaMemcpy(host_Distances, device_Distances, size_Distances, cudaMemcpyDeviceToHost) );





    // deallocation
    CUDA_CHECK( cudaFree(device_EdgeArray) );
    CUDA_CHECK( cudaFree(device_Distances) );

    free(host_EdgeArray);

    return host_hasNegativeCycle;
}






/// @brief 
/// @param argv[0] -> The executable's name includes the optimization with which it was compiled  
/// @param argv[1] -> number of vertices
/// @param argv[2] -> number of OpenMP threads 
/// @param argv[3] -> cuda tipe : 0 = shared memory L1 < sh; 1 = no shared memory L1 > sh; 2 = shared memory L1 >sh
/// @param argv[4] -> vertexs source number
/// @param argv[5] -> filename = File from which to read the matrix
/// @param argv[6] -> filename2 = File to write the results to
/// @return 
int main(int argc, char *argv[]){

    //OpenMP
    int num_threads = 1;
    int tipe = 0;
    //---

    //For testing
    struct timeval total_time_Start, total_time_End; 
    struct timeval algorithm_Start, algorithm_End; 
    struct timeval input_time_Start, input_time_End;
    struct timeval output_time_Start, output_time_End;
    double total_time; 
    double input_time; 
    double algorithm ; 
    double output_time;
    int opt = -1;
    //---

    //for BF
    char fileName[50];
    char fileName2[50];
    int V = DEFAULT_V; 
    int * graph1D;
    int sourceVertex = DEFAULT_SOURCE;
    int * dist; // Minimum distance from the source to each vertex
    int negativeCicle = -1;
    //---


    //TIME
    gettimeofday(&total_time_Start,NULL);



    //Arg
    for (int i = strlen(argv[0]); argv[0][i] != 'n'; i--) { //'n' from main
        if (isdigit(argv[0][i])) {
            opt = atoi(&argv[0][i]);
            break;  
        }
    }
    if(opt==-1){
        fprintf(stderr, "name of the program:%s, and opt: %d",argv[0],opt);
        exit(EXIT_FAILURE);
    }

    if(argc > 1){
        if (sscanf(argv[1], "%u", &V) == 1) {
        } else {
            fprintf(stderr, "vertexs number (1° arg) expected as number, instead it was \"%s\"\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 2){
        if (sscanf(argv[2], "%d", &num_threads) == 1) {
            //OpenMP
            omp_set_num_threads(num_threads);
            //---
        } else {
            fprintf(stderr, "number of threads (2° arg) expected as number, instead it was \"%s\"\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 3){
        if (sscanf(argv[3], "%d", &tipe) == 1) {
        } else {
            fprintf(stderr, "cuda tipe (3° arg) expected as number, instead it was \"%s\"\n", argv[3]);
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 4){
        if (sscanf(argv[4], "%u", &sourceVertex) == 1) {
        } else {
            fprintf(stderr, "vertexs source number (4° arg) expected as number, instead it was \"%s\"\n", argv[4]);
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 5){
        strcpy(fileName, argv[5]);
    }
    else{
        strcpy(fileName, DEFAULT_FILENAME1D);
    }         

    if(argc > 6){
        strcpy(fileName2, argv[6]);
    }
    else{
        strcpy(fileName2, DEFAULT_FILENAME_BellmanFord_Cuda);
    }



//-------------------------------------
    
      
 
   
    int blocksize = MAX_DIMBLOCK;


    //TIME
    gettimeofday(&input_time_Start,NULL);


    graph1D=(int*) malloc(V*V*sizeof(int));
    if(graph1D== NULL){
        fprintf(stderr,"malloc error");
    }

    
    //Reading the binary file without data partitioning
    if(readGraphFromFile1D(graph1D, V, fileName)!=0){
        fprintf(stderr,"reading file error");
        exit(EXIT_FAILURE); 
    }


    //TIME
    gettimeofday(&input_time_End,NULL); 
    



/*BellmanFord------------------------------------------------*/
    
    //TIME
    gettimeofday(&algorithm_Start,NULL);



    //BellmanFord
    dist = (int*)malloc(V * sizeof(int));
    if(dist == NULL){
        fprintf(stderr,"malloc error\n");
        exit(EXIT_FAILURE);
    }



 
    //calling bellmanFord
    int totalThread=0;
    negativeCicle = bellmanFord_cuda(graph1D , V,  sourceVertex, dist, blocksize, &totalThread, tipe );




    gettimeofday(&algorithm_End,NULL);
 


/*print BellmanFord----------------------------------------*/

    

    gettimeofday(&output_time_Start,NULL);//TIME



    if(negativeCicle == 0){
        print_bellmanFord_sequential_txt(fileName2,V, dist, NULL, sourceVertex);
    }else if(negativeCicle == 1){
        print_bellmanFord_negativeCicle(fileName2);
    }else{
        fprintf(stderr,"BellmanFord error\n");
    }




    gettimeofday(&output_time_End,NULL); //TIME



/*ending-----------------------------*/

    //free
    free(graph1D);
    free(dist);



    gettimeofday(&total_time_End,NULL); //TIME
        

    total_time = timeval_to_microseconds(&total_time_Start, &total_time_End); 
    input_time = timeval_to_microseconds(&input_time_Start, &input_time_End); 
    algorithm = timeval_to_microseconds(&algorithm_Start, &algorithm_End); 
    output_time = timeval_to_microseconds(&output_time_Start, &output_time_End);


    char risultato[30];
    sprintf(risultato, "%s_%d", "OMP+CUDA", tipe);
    if(printCSV(risultato, V, opt, num_threads, totalThread, total_time, algorithm, input_time, output_time)!=0){
        fprintf(stderr,"CSV error");
        exit(EXIT_FAILURE);
    }

    return 0;

    

}











/////////////////////////////////////////////////////////////////////////
int print_bellmanFord_sequential_txt(char* fileName, int V, int* dist, int* parent, int sourceVertex) {
    char *filenameV = (char *)malloc(strlen(fileName) + sizeof(char)*5);
    strcpy(filenameV, fileName);
    char stringV[10];
    sprintf(stringV, "%d", V);
    strcat(filenameV, stringV);

    char *filename_with_extension = (char *)malloc(strlen(filenameV) + sizeof(char)*5);
    strcpy(filename_with_extension, filenameV);
    strcat(filename_with_extension, ".txt");



    FILE* file = fopen(filename_with_extension, "w");
    if (file == NULL) {
        fprintf(stderr,"file error");
        return -1;
    }

    
    int i;
    for (i = 0; i < V; i++) {
        fprintf(file, "Minimum distance from source to vertex %d: %d\t", i, dist[i]);
        fprintf(file, "\n");
    }

    // Chiudi il file
    fclose(file);
    return 0;
}




///////////////////////////////////////////////////////////////////////////////////////////
int adjacencyMatrixGen1D(int V, int weightMax, int weightMin , float edgeProbability, unsigned int seed, int *graph) {
    
    int weight, i, j, temp;

/*--------------------------------------------------------------------*/

    //Graph generator
    srand(seed);
    
    for (i = 0; i < V; i++) {
        for (j = 0; j < V; j++) {
        
            if (((double)rand() / RAND_MAX) < edgeProbability) {
                weight = rand() % (weightMax - weightMin + 1) + weightMin;


                //we eliminate the obvious negative cycles (not guarantee the absence of negative cycles)
                if(weight < 0){

                    // We avoid the creation of a negative cycle originating from the same vertex. 
                    if(i == j){ 
                        weight = abs(weight);
                    }

                    // We avoid the creation of a negative cycle between two nodes
                    else if(graph[j*V+i] != ABSENTE_EDGE && graph[j*V+i] + weight <0 ){
                        temp = abs(graph[j*V+i]) + 1;
                        if(weightMax > temp )
                            weight = temp;
                        else
                            weight = weightMax;
                    }

                    
                        
                }


                graph[i*V+j] = weight;
                

            } else {
                graph[i*V+j] = ABSENTE_EDGE;
            }
            
        }
    }

    return 0;

}

///////////////////////////////////////////////////////////////////////////////////////////
int printGraph1D(int * graph, int V) {
    int x=ABSENTE_EDGE;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            x=graph[i*V+j];
            if(x==ABSENTE_EDGE) 
                printf("x\t");
            else 
                printf("%d\t", x);
        }
        printf("\n");
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
char * filenameInizialzie(char fileName [], int V){
    char *filenameV = (char *)malloc(strlen(fileName) + sizeof(char)*5);
    if(filenameV==NULL){
        fprintf(stderr,"malloc error\n");
        exit(EXIT_FAILURE);
    }
    strcpy(filenameV, fileName);
    char stringV[10];
    sprintf(stringV, "%d", V);
    strcat(filenameV, stringV);

    char *filename_with_extension = (char *)malloc(strlen(filenameV) + sizeof(char)*5);
    if(filename_with_extension==NULL){
        fprintf(stderr,"malloc error\n");
        exit(EXIT_FAILURE);
    }
    strcpy(filename_with_extension, filenameV);
    strcat(filename_with_extension, ".bin");
    free(filenameV);
    return filename_with_extension;
}

///////////////////////////////////////////////////////////////////////////////////////////
int print_bellmanFord_negativeCicle(char* fileName){
    char *filename_with_extension = (char *)malloc(strlen(fileName) + sizeof(char)*5);
    strcpy(filename_with_extension, fileName);
    strcat(filename_with_extension, ".txt");

    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        fprintf(stderr,"file error\n");
        return -1;
    }

    fprintf(file, "The graph contains a negative cycle\n");

    fclose(file);
    return 0;
}

/////////////////////////////////////////////////////////////////////////
int readGraphFromFile1D(int *graph, int V, char *filename) {
    char *filenameV = (char *)malloc(strlen(filename) + sizeof(char)*5);
    if(filenameV == NULL){
        fprintf(stderr, "malloc file error");
        return -1;
    }
    strcpy(filenameV, filename);
    char stringV[10];
    sprintf(stringV, "%d", V);
    strcat(filenameV, stringV);

    char *filename_with_extension = (char *)malloc(strlen(filenameV) + sizeof(char)*5);
    if(filename_with_extension == NULL){
        fprintf(stderr, "malloc file error");
        return -1;
    }
    strcpy(filename_with_extension, filenameV);
    strcat(filename_with_extension, ".bin");

    FILE *file = fopen(filename_with_extension, "rb");

    if (file == NULL) {
        fprintf(stderr, "File %s could not be opened for reading.\n", filename_with_extension);
        return -1;
    }


    if(fread(graph, sizeof(int), V*V, file) != V*V){
        fprintf(stderr, "Error reading binary file.\n");
        fclose(file);
        return -1;
    }


    free(filename_with_extension);
    free(filenameV);
    fclose(file); 
    return 0;
}

/////////////////////////////////////////////////////////////////////////
double timeval_to_microseconds(struct timeval *time1, struct timeval *time2) {
    unsigned long long temp1 = (unsigned long long)time1->tv_sec * 1000000 + time1->tv_usec;
    unsigned long long temp2 = (unsigned long long)time2->tv_sec * 1000000 + time2->tv_usec;

    return (double)(temp2 - temp1)/(double)1000000;
}

/////////////////////////////////////////////////////////////////////////
int printCSV(const char tipe[],int V, int opt, int numThreads, int size, double total_time, double algorithm, double input_time, double output_time){
    
    
    FILE *fp2; 
    char path2[200];

    if(strcmp(tipe, "OMP+MPI") == 0){
        sprintf(path2, "Informations/OMP_MPI/opt%d/%dvertices.csv", opt, V); 
    }else if(strcmp(tipe, "OMP+CUDA_0") == 0 ||strcmp(tipe, "OMP+CUDA_1") == 0 || strcmp(tipe, "OMP+CUDA_2") == 0 ){
        sprintf(path2, "Informations/OMP_CUDA/opt%d/%dvertices.csv", opt, V);   
    }else if(strcmp(tipe, "none") == 0){
        sprintf(path2, "Informations/SEQUENTIAL/opt%d/%dvertices.csv", opt, V);
    }else{
         fprintf(stderr, "Unexpected compilation type: %s\n", tipe);
         return -2;
    }
    char *filename2 = path2;
    fp2 = fopen(filename2, "a+");
    if (fp2 == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", filename2);
        return -1;
    }
    fprintf(fp2, "%s;%d;%d;%06f;%06f;%06f;%06f;\n", tipe, numThreads, size, total_time, algorithm, input_time, output_time);
    fclose(fp2);
    return 0;
}



