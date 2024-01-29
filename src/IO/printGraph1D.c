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

#include "../toInclude/IO/printGraph.h"


/// @brief Prints a graph
/// @param graph  adjacency matrix as vector
/// @param V number of vertices
/// @return If errors have occurred
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