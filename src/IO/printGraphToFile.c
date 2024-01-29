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

#include "../toInclude/IO/printGraphToFile.h"


/// @brief Prints a graph on file
/// @param graph adjacency matrix as vector
/// @param V number of vertices
/// @param filename Name of the file to create
/// @return If errors have occurred
int printGraphToFile(int ** graph, int V, char *filename) {
    //change filename for number of vertices
    char *filenameV = (char *)malloc(strlen(filename) + sizeof(char)*5);
    strcpy(filenameV, filename);
    char stringV[10];
    sprintf(stringV, "%d", V);
    strcat(filenameV, stringV);

    char *filename_with_extension = (char *)malloc(strlen(filenameV) + sizeof(char)*5);
    strcpy(filename_with_extension, filenameV);
    strcat(filename_with_extension, ".bin");
    
    FILE *file = fopen(filename_with_extension, "wb");  

    if (file == NULL) {
        fprintf(stderr, "File %s could not be opened for writing.\n", filename_with_extension);
        return -1;
    }

    
    for (int i = 0; i < V; i++) {
        if(fwrite(graph[i], sizeof(int), V, file) != V){
            fprintf(stderr, "Error writing to binary file.\n");
            fclose(file);
            exit(EXIT_FAILURE);
       }
    }


    free(filename_with_extension);
    fclose(file);
    return 0; 
}