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
#include "toInclude/bellmanFordSequential.h"



int print_bellmanFord_negativeCicle(char*);//writes to the file that there is a negative loop


/// @brief This represents the 'baseline' version without any optimization
/// @param argv[0] -> The executable's name includes the optimization with which it was compiled  
/// @param argv[1] -> number of vertices
/// @param argv[3] -> vertexs source number
/// @param argv[4] -> filename = File from which to read the matrix
/// @param argv[5] -> filename2 = File to write the results to
int main(int argc, char *argv[]) {

    char fileName[50];
    char fileName2[50];
    int V = DEFAULT_V; 
    
    int ** graph;
    int i, j;

    int sourceVertex = DEFAULT_SOURCE;
    int * dist; // Minimum distance from the source to each vertex
    int * parent; // Predecessor vertex in the shortest path
    int negativeCicle = -1;
    
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
        fprintf(stderr, "name of the program:%s, and opt: %d",argv[0],opt);;
        exit(EXIT_FAILURE);

    }

    if(argc > 1){
        if (sscanf(argv[1], "%u", &V) == 1) {
        } else {
            fprintf(stderr, "number of vertices (1° arg) expected as number, instead it was \"%s\"\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 2){
        if (sscanf(argv[2], "%u", &sourceVertex) == 1) {
        } else {
            fprintf(stderr, "vertexs source number (2° arg) expected as number, instead it was \"%s\"\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }
        
    if(argc > 3){
        strcpy(fileName, argv[3]);
    }
    else{
        strcpy(fileName, DEFAULT_FILENAME);
    }         

    if(argc > 4){
        strcpy(fileName2, argv[4]);
    }
    else{
        strcpy(fileName2, DEFAULT_FILENAME_BellmanFord);
    }




/*Reading-----------------------------------------------------------*/
    
    //TIME
    gettimeofday(&input_time_Start,NULL); 
    graph = (int**)malloc(V * sizeof(int *));
    if(graph == NULL){
        fprintf(stderr,"malloc error");
        exit(EXIT_FAILURE);
    }
    for(j=0; j<V; j++){
        graph[j] = (int*)malloc(V * sizeof(int));
        if(graph[j] == NULL){
            fprintf(stderr,"malloc error");
            exit(EXIT_FAILURE);
        }
        for(i=0; i<V; i++){
            graph[j][i] = ABSENTE_EDGE;
        }
    }

    
    //read
    if(readGraphFromFile(graph, V, fileName)!=0){
        fprintf(stderr,"reading file error");
        exit(EXIT_FAILURE); 
    }

    //TIME
    gettimeofday(&input_time_End,NULL);



/*BellmanFord--------------------------------------------------------*/

    //TIME
    gettimeofday(&algorithm_Start,NULL);


    //BellmanFord
    dist = (int*)malloc(V * sizeof(int));
    if(dist == NULL){
        fprintf(stderr,"malloc error");
        exit(EXIT_FAILURE);
    }

    parent = (int*)malloc(V * sizeof(int));
    if(parent == NULL){
        fprintf(stderr,"malloc error");
        exit(EXIT_FAILURE);
    }


    negativeCicle= bellmanFord_sequential2D(graph, V, sourceVertex,  dist, parent);


    gettimeofday(&algorithm_End,NULL);



/*print BellmanFord----------------------------------------------------------*/
    //Time
    gettimeofday(&output_time_Start,NULL);

    if(negativeCicle == 0){
        print_bellmanFord_sequential_txt(fileName2, V, dist, parent);
        #ifdef TESTING
            if(V<50){
                print_bellmanFord_sequential(V, dist, parent);
            }else{
                printf("--bellmanFord too large to print\n");
            }
            
        #endif
    }else if(negativeCicle == 1){
        print_bellmanFord_negativeCicle(fileName2);
        #ifdef TESTING
        printf("The graph contains a negative cycle\n");
        #endif
    }else{
        fprintf(stderr,"BellmanFord error\n");
    }

    //Time
    gettimeofday(&output_time_End,NULL);

/*Ending----------------------------------------------------------*/

    //free
    for(j=0; j<V; j++){
        free(graph[j]);
    }
    free(graph);
    free(dist);
    free(parent);

    gettimeofday(&total_time_End,NULL);


    total_time = timeval_to_microseconds(&total_time_Start, &total_time_End); 
    input_time = timeval_to_microseconds(&input_time_Start, &input_time_End); 
    algorithm = timeval_to_microseconds(&algorithm_Start, &algorithm_End); 
    output_time = timeval_to_microseconds(&output_time_Start, &output_time_End);
    if(printCSV("none", V, opt, 0, 0, total_time, algorithm, input_time, output_time)!=0){
        fprintf(stderr,"CSV error");
        exit(EXIT_FAILURE);
    }



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
        fprintf(stderr,"file error");
        return -1;
    }

    fprintf(file, "The graph contains a negative cycle");

    fclose(file);
    return 0;
}