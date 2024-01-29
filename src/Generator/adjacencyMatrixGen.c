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

#include "../toInclude/Generator/adjacencyMatrixGen.h"



/// @brief Generates an adjacency matrix
/// @param V Number of vertices
/// @param weightMax Maximum weight
/// @param weightMin Minimum weight
/// @param edgeProbability Probability of the presence of an edge
/// @param seed For random generation
/// @param graph Graph pointer
/// @return If errors have occurred
int adjacencyMatrixGen(int V, int weightMax, int weightMin , float edgeProbability, unsigned int seed, int **graph) {
    
    
    int weight, i, j, temp;
   

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
                    else if(graph[j][i] != ABSENTE_EDGE && graph[j][i] + weight <0 ){
                        temp = abs(graph[j][i]) + 1;
                        if(weightMax > temp )
                            weight = temp;
                        else
                            weight = weightMax;
                    }
                    
                        
                }


                graph[i][j] = weight;
                

            } else {
                graph[i][j] = ABSENTE_EDGE;
            }
            
        }
    }


    /*
    free(vertexNegativeIn);
    free(vertexNegativeOut);
    */


/*--------------------------------------------------------------------*/


    return 0;
}













