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

#include "../toInclude/IO/printGraphToFileAsTxt1D.h"


/// @brief Prints a graph on file txt
/// @param graph adjacency matrix as vector
/// @param V number of vertices
/// @param filename Name of the file to create
/// @return If errors have occurred
int printGraphToFileAsTxt1D(int * graph, int V, char filename[]) {

    //V
    char *filenameV = (char *)malloc(strlen(filename) + sizeof(char)*5);
    if(filenameV == NULL){
        fprintf(stderr, "malloc error");
        return -1;
    }
    strcpy(filenameV, filename);
    char stringV[10];
    sprintf(stringV, "%d", V);
    strcat(filenameV, stringV);

    //.txt
    char *filename_with_extension = (char *)malloc(strlen(filenameV) + sizeof(char)*5);
    if(filename_with_extension == NULL){
        fprintf(stderr, "malloc error");
        return -1;
    }
    strcpy(filename_with_extension, filenameV);
    strcat(filename_with_extension, ".txt");


    
    FILE *file = fopen(filename_with_extension, "w");  

    if (file == NULL) {
        fprintf(stderr, "File %s could not be opened for writing.\n", filename_with_extension);
        return -1;
    }

    fprintf(file, "\t");
    for (int i= 0; i<V; i++){
        fprintf(file, "V%d\t", i);
    }
    fprintf(file, "\n");
    for ( int i = 0; i < V; i++) {
        for ( int j = 0; j < V; j++) {
            if(j==0){
                fprintf(file, "V%d\t", i);
            }
            if(graph[i*V+j]!=ABSENTE_EDGE){
            fprintf(file, "%d\t", graph[i*V+j]);
            }
            else{
                fprintf(file, "x\t");
            }
        }
        fprintf(file, "\n"); // Vai a capo alla fine di ogni riga
    }

    free(filename_with_extension);
    //free(filename_1D);
    fclose(file);
    return 0;  
}