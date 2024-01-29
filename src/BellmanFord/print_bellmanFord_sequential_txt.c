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


#include "../toInclude/BellmanFord/print_bellmanFord_sequential_txt.h"



/// @brief Writes the result of the algorithm
/// @param fileName Name of the file to print the result onto.
/// @param V Number of vertices
/// @param dist Vector of distances
/// @param parent Vector of parents
/// @return If errors have occurred
int print_bellmanFord_sequential_txt(char* fileName, int V, int* dist, int* parent) {
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
        if(parent!=NULL){
            printPath_txt(file, parent, i);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

//Recursive function to write the paths
void printPath_txt(FILE * file, int* parent, int i) {
    if (parent[i] == INF) {
        fprintf(file,"%d", i);
        return;
    }

    printPath_txt(file, parent, parent[i]);
    fprintf(file," -> %d", i);
}