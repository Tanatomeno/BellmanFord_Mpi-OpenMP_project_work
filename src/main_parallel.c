/*
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

#include "toInclude/main.h"
#include "toInclude/io.h"
#include "toInclude/bellmanFordParallel.h"



int print_bellmanFord_negativeCicle(char*); //writes to the file that there is a negative loop
char * filenameInizialzie(char[], int ); //change the name of the file by adding the number of elements and the extension

#define binaryWriting // If defined, writes to a binary file in a parallel manner
//#define Test //If defined, checks that the binary file has been written correctly

/// @param argv[0] -> The executable's name includes the optimization with which it was compiled  
/// @param argv[1] -> number of vertices
/// @param argv[2] -> number of OpenMP threads 
/// @param argv[3] -> vertexs source number
/// @param argv[4] -> filename = File from which to read the matrix
/// @param argv[5] -> filename2 = File to write the results to
int main(int argc, char *argv[]) {

    
    //MPI initialization
    MPI_Init(&argc, &argv);
    MPI_Comm comm;
    int nProcess;//number of processors
    int myRank;//my global rank
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &nProcess);
    MPI_Comm_rank(comm, &myRank);
    //---

    //OpenMP
    int num_threads = 1;
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
    int len;
    int myStart;
    int myEnd;

    char fileName[50];
    char fileName2[50];
    int V = DEFAULT_V; 
    
    int * graph1D;

    int sourceVertex = DEFAULT_SOURCE;
    int * dist; // Minimum distance from the source to each vertex
    int negativeCicle = -1;


if(myRank==0){gettimeofday(&total_time_Start,NULL);}//TIME

    //Arg
    for (int i = strlen(argv[0]); argv[0][i] != 'n'; i--) { //'n' from main
        if (isdigit(argv[0][i])) {
            opt = atoi(&argv[0][i]);
            break;  
        }
    }
    if(opt==-1){
        fprintf(stderr, "name of the program:%s, and opt: %d",argv[0],opt);
        MPI_Finalize();
        exit(EXIT_FAILURE);

    }

    if(argc > 1){
        if (sscanf(argv[1], "%u", &V) == 1) {
        } else {
            if(myRank==0){
                fprintf(stderr, "number of vertices (1° arg) expected as number, instead it was \"%s\"\n", argv[1]);
            }
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 2){
        if (sscanf(argv[2], "%d", &num_threads) == 1) {
            
        } else {
            if(myRank==0){
                fprintf(stderr, "number of threads (2° arg) expected as number, instead it was \"%s\"\n", argv[2]);
            }
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 3){
        if (sscanf(argv[3], "%u", &sourceVertex) == 1) {
        } else {
            if(myRank==0){
                fprintf(stderr, "vertexs source number (2° arg) expected as number, instead it was \"%s\"\n", argv[3]);
            }
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    }
        
    if(argc > 4){
        strcpy(fileName, argv[4]);
    }
    else{
        strcpy(fileName, DEFAULT_FILENAME1D);
    }         

    if(argc > 5){
        strcpy(fileName2, argv[5]);
    }
    else{
        strcpy(fileName2, DEFAULT_FILENAME_BellmanFord_Parallel);
    }


/*File division--------------------------------------------------------------------*/

    //OpenMP!
    omp_set_num_threads(num_threads);




    //Distribute the matrix rows among the processes
    len = (int) V / nProcess;
    myStart = len * myRank;
    myEnd = len * (myRank + 1); //(end not included)
    if (myRank == nProcess - 1) { 
        myEnd = V;
    }



    //TIME
    if(myRank==0){gettimeofday(&input_time_Start,NULL);} 



    //MPI
    MPI_Barrier(comm);
    //---

    //Reading file
    char*filename_with_extension= filenameInizialzie(fileName, V); 
    MPI_File filempi;
    int openResult = MPI_File_open(comm, filename_with_extension, MPI_MODE_RDONLY,MPI_INFO_NULL, &filempi);
    if(openResult!=MPI_SUCCESS){
        fprintf(stderr,"opening file error\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    
    //malloc my part of the matrix
    graph1D=(int*) malloc((myEnd-myStart)*V*sizeof(int));
    if(graph1D==NULL){
        fprintf(stderr,"malloc error\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    //reading my part of the matrix
    MPI_File_read_at_all(filempi,(myStart*V)*sizeof(int), graph1D, (myEnd-myStart)*V, MPI_INT, MPI_STATUS_IGNORE);

    MPI_File_close(&filempi);


    //TIME
    if(myRank==0){gettimeofday(&input_time_End,NULL);}


    //MPI
    MPI_Barrier(comm);
    //---




/*BellmanFord---------------------------------------------------------------------*/
    
    //TIME
    if(myRank==0){gettimeofday(&algorithm_Start,NULL);}//TIME



    //Distance vector
    dist = (int*)malloc(V * sizeof(int));
    if(dist == NULL){
        fprintf(stderr,"malloc error\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    #ifdef PARENTS
        //Parent vector
        int* parent = (int*)malloc(V * sizeof(int));
        if(parent == NULL){
            fprintf(stderr,"malloc error\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    #else
        int* parent =NULL;
    #endif


    
    //MPI
    MPI_Barrier(comm);
    //---

    //BellmanFord
    negativeCicle = bellmanFord_parallel(graph1D, V, sourceVertex,  dist, parent, comm, myStart, myEnd);

    

    //MPI
    MPI_Barrier(comm);
    //---


    //TIME
    if(myRank==0){gettimeofday(&algorithm_End,NULL);}
 


/*Print BellmanFord--------------------------------------------------------------------*/
    
    //TIME
    if(myRank==0){gettimeofday(&output_time_Start,NULL);} 



    #ifdef binaryWriting
        filename_with_extension= filenameInizialzie(fileName2, V);
        openResult = MPI_File_open(comm, filename_with_extension, MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL, &filempi);
        if(openResult!=MPI_SUCCESS){
            fprintf(stderr,"opening output file error\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }


        //Each process writes a portion of the distances file (same for all processes)
        MPI_File_write_ordered(filempi, &dist[myStart], (myEnd-myStart), MPI_INT, MPI_STATUS_IGNORE);
        if(negativeCicle==1 && (myRank==0)){

            const int flag = 123456789; //"If there is a negative cycle in the binary file, process 0 writes a flag number
           MPI_File_write(filempi, &flag, 1, MPI_INT, MPI_STATUS_IGNORE);

        }else if(negativeCicle!=0){
            fprintf(stderr,"BellmanFord error\n");
            MPI_Finalize();
            exit(EXIT_FAILURE); 
        }

        MPI_File_close(&filempi);


        #ifdef Test
            if(myRank == 0){

                if(negativeCicle == 1){
                    printf("negative cycle\n");
                }else if(negativeCicle!=0){
                    fprintf(stderr,"BellmanFord error\n");
                    MPI_Finalize();
                    exit(EXIT_FAILURE);
                }
                

                FILE * file = fopen(filename_with_extension, "rb");
                if (file == NULL) {
                    fprintf(stderr, "Unable to open file .bin in teasting %s\n", filename_with_extension);
                    return 1;
                }

                if(fread(dist, sizeof(int), V, file) != V){
                    fprintf(stderr, "reading error .bin in teasting %s\n", filename_with_extension);
                    return 1;
                }

                for(int i=0;i<V;i++){
                    printf("%d: %d\n",i,dist[i]);
                }
            }

            if(myRank == 0){
                if(negativeCicle == 0){
                    print_bellmanFord_sequential_txt(fileName2, V, dist,parent);
                }else if(negativeCicle == 1){
                    print_bellmanFord_negativeCicle(fileName2);
                }else{
                    fprintf(stderr,"BellmanFord error\n");
                    MPI_Finalize();
                    exit(EXIT_FAILURE);
                }
            }

        #endif

    #else
        if(myRank == 0){
            if(negativeCicle == 0){
                print_bellmanFord_sequential_txt(fileName2, V, dist,parent);
            }else if(negativeCicle == 1){
                print_bellmanFord_negativeCicle(fileName2);
            }else{
                fprintf(stderr,"BellmanFord error\n");
                MPI_Finalize();
                exit(EXIT_FAILURE);
            }
        }
    #endif





    //TIME
    if(myRank==0){gettimeofday(&output_time_End,NULL);} 



/*Ending------------------------------------------------------------*/

    //free
    free(graph1D);
    free(dist);
    #ifdef PARENTS
        free(parent);
    #endif



    //MPI
    MPI_Barrier(comm);
    //---

    if(myRank==0){gettimeofday(&total_time_End,NULL);} //TIME
        
    if(myRank==0){
        total_time = timeval_to_microseconds(&total_time_Start, &total_time_End); 
        input_time = timeval_to_microseconds(&input_time_Start, &input_time_End); 
        algorithm = timeval_to_microseconds(&algorithm_Start, &algorithm_End); 
        output_time = timeval_to_microseconds(&output_time_Start, &output_time_End);

        //Wtring time on CSV
        if(printCSV("OMP+MPI", V, opt, num_threads, nProcess, total_time, algorithm, input_time, output_time)!=0){
            fprintf(stderr,"CSV error");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
    }

    MPI_Finalize();
    return 0;
 

}



/// @brief It writes to the file that there is a negative cycle in the graph
/// @param fileName Where to write
/// @return If errors have occurred
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


/// @brief change the name of the file by adding the number of elements and the extension
/// @param fileName Where to write
/// @param V number of elements
/// @return If errors have occurred
char * filenameInizialzie(char fileName [], int V){
    char *filenameV = (char *)malloc(strlen(fileName) + sizeof(char)*5);
    if(filenameV==NULL){
        fprintf(stderr,"malloc error\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    strcpy(filenameV, fileName);
    char stringV[10];
    sprintf(stringV, "%d", V);
    strcat(filenameV, stringV);

    char *filename_with_extension = (char *)malloc(strlen(filenameV) + sizeof(char)*5);
    if(filename_with_extension==NULL){
        fprintf(stderr,"malloc error\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    strcpy(filename_with_extension, filenameV);
    strcat(filename_with_extension, ".bin");
    free(filenameV);
    return filename_with_extension;
}