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


#include "../toInclude/BellmanFord/print_bellmanFord_serial.h"




/// @brief Writes the result of the algorithm
/// @param V Number of vertices
/// @param dist Vector of distances
/// @param parent Vector of parents
/// @return If errors have occurred
int print_bellmanFord_serial(int V, int * dist, int * parent){
    int i;
    for (i = 0; i < V; i++) {
        printf("Minimum distance from source to vertex %d: %d\t\t ", i, dist[i]);
        if(parent!=NULL){
            printPath(parent, i);
        }
        printf("\n");
    }
    return 0;
}



//Recursive function to write the paths
void printPath(int* parent, int i) {
    if (parent[i] == INF) {
        printf("%d", i);
        return;
    }

    printPath(parent, parent[i]);
    printf(" -> %d", i);
}