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
#include "toInclude/generator.h"


#define ALL  //Whether to use only a 2D generation or also a 1D (they are equivalent)


/// @param argv[0] -> Not used
/// @param argv[1] -> number of vertices
/// @param argv[2] -> seed
/// @param argv[3] -> Probability that there is an edge between two vertices
/// @param argv[4] -> Maximum weight a of the edges
/// @param argv[5] -> Minimum weight of the edges
/// @param argv[6] -> filename = Output file name to which the number of vertices and the extension will be added
int main(int argc, char *argv[]) {

    char fileName[50];
    int V = DEFAULT_V; 
    unsigned int seed = time(NULL); 
    float edgeProbability = DEFAULT_EDGE_PROBABILITY;
    int weightMax = DEFAULT_WEIGHT_MAX;
    int weightMin = DEFAULT_WEIGHT_MIN; 


   

    if(argc > 1){
        if (sscanf(argv[1], "%d", &V) == 1) {
        } 
        else {
            fprintf(stderr, "number of vertices (1° arg) expected as number, instead it was \"%s\"\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 2){
        if (sscanf(argv[2], "%u", &seed) == 1) {
        } 
        else {
            fprintf(stderr, "Seed (2° arg) expected as number, instead it was \"%s\"\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 3){
        if (sscanf(argv[3], "%f", &edgeProbability) == 1) {
        } 
        else {
            fprintf(stderr, "Edge Probability (3° arg) expected as number, instead it was \"%s\"\n", argv[3]);
            exit(EXIT_FAILURE);
        }
    }

    if(argc > 5){
        if (sscanf(argv[4], "%u", &weightMax) == 1 && sscanf(argv[5], "%u", &weightMin)) {
        } 
        else {
            fprintf(stderr, "Weight Max (4° arg) and Weight Min (5° arg) expected as number, instead there were \"%s\" and \"%s\"\n", argv[4], argv[5]);
            exit(EXIT_FAILURE);
        }
    }


    if(argc > 6){
        strcpy(fileName, argv[6]);
    }
    else{
        strcpy(fileName, DEFAULT_FILENAME);
    }








/*---------------------------------------------------------*/


    int ** graph;
    int i,j;

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

    if(adjacencyMatrixGen(V, weightMax, weightMin , edgeProbability,  seed, graph)!=0) {
        exit(EXIT_FAILURE);
    }

    if(printGraphToFileAsTxt(graph, V, fileName)!= 0){
        exit(EXIT_FAILURE);
    }


    if(printGraphToFile(graph, V, fileName)!= 0){
         exit(EXIT_FAILURE);
    }

    free (graph);


#ifdef ALL

    int * graph1D;    
    //1D
    graph1D = (int *)malloc(V * V * sizeof(int));
    if(graph1D == NULL){
        fprintf(stderr,"malloc error");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<V*V; i++){
            graph1D[i] = ABSENTE_EDGE;
    }



    if(adjacencyMatrixGen1D(V, weightMax, weightMin , edgeProbability,  seed, graph1D)!=0) {
        exit(EXIT_FAILURE);
    }

#ifndef ALL

    if(printGraphToFileAsTxt1D(graph1D, V, fileName)!= 0){
        exit(EXIT_FAILURE);
    }

#endif


    if(printGraphToFile1D(graph1D, V, fileName)!= 0){
        exit(EXIT_FAILURE);
    }
    

    free(graph1D);


#endif 

#ifdef TESTING
    //resetting matrix
    for(i=0; i<V; i++){
        for(j=0; j<V; j++){
            graph[i][j] = ABSENTE_EDGE;
        }
    }
    if(readGraphFromFile(graph, V, fileName)!= 0){
        exit(EXIT_FAILURE);
    }
    if(readGraphFromFile1D(graph1D, V, fileName)!= 0){
        exit(EXIT_FAILURE);
    }
    if(V<50){
        if(printGraph(graph, V)!=0){
            exit(EXIT_FAILURE);
        }
        printf("\n");
        if(printGraph1D(graph1D, V)!=0){
            exit(EXIT_FAILURE);
        }
    }else{
        printf("--graph too large to print\n");
    }
#endif

  return 0;


}