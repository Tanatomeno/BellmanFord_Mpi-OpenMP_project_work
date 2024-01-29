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

#include "../toInclude/BellmanFord/bellmanFord_sequential.h"


//To be defined if we also want to find the path of parents
//#define PARENTS 



/// @brief  Original Bellman-Ford algorithm.
///         In this version, in addition to finding the minimum path, it also stores the traversed vertices
/// @param graph The 1D graph to work on
/// @param V Number of vertices in the graph
/// @param src Starting vertex identified by a number
/// @param dist Vector of distances passed from the main
/// @param parent Vector of parents passed from the main
/// @return Presence of negative cycles
int bellmanFord_sequential(int* graph, int V, int src, int * dist, int* parent) {

    int i, j, temp, count;
    int toUpdate = 0;
    int iterNumber = 0;
    

    // SetUp
    for (i = 0; i < V; i++) {
        dist[i] = INF;
    }
    dist[src] = 0; // dist for src to src is 0


    #ifdef PARENTS
        for (i = 0; i < V; i++) {
            parent[i] = ABSENTE_EDGE;
        }
    #endif


    

    // Relaxation of the strings |V| - 1 times
    for (count = 0; count < V - 1; count++) {

        toUpdate = 0; //Variable indicating whether it is necessary to proceed with further iterations
        iterNumber++;

        for (i = 0; i < V; i++) {
            for (j = 0; j < V; j++) {
                if (graph[i*V+j] != ABSENTE_EDGE) {
                    if(dist[i]!=INF){ // we must avoid adding a value to the maximum representable value, otherwise it will be interpreted as a negative value after the addition
                        temp = dist[i] + graph[i*V+j];
                        if (dist[j] > temp  ) {
                            dist[j] = temp;

                            #ifdef PARENTS
                                parent[j] = i;
                            #endif

                            toUpdate = 1;

                        }
                    }
                }
            }
        }

    
        // No updates during this iteration, we're exiting early
        if (toUpdate!=1){
            break;
        }
    
    }

    // Simply perform another iteration and check if an even smaller path is found (which is not possible if there are no negative cycles)
    if(iterNumber == V-1 ){
        for (i = 0; i < V; i++) {
            for (j = 0; j < V; j++) {
                if (graph[i*V+j] != ABSENTE_EDGE && dist[i]!=INF && dist[j] > dist[i] + graph[i*V+j]) {
                    return 1;
                }
            }
        }
    }



    return 0;

    
}




/// @brief  Original Bellman-Ford algorithm.
///         In this version, in addition to finding the minimum path, it also stores the traversed vertices
/// @param graph The 2D graph to work on
/// @param V Number of vertices in the graph
/// @param src Starting vertex identified by a number
/// @param dist Vector of distances passed from the main
/// @param parent Vector of parents passed from the main
/// @return Presence of negative cycles
int bellmanFord_sequential2D(int** graph, int V, int src, int * dist, int* parent) {

    int i, j, temp, count;
    int toUpdate = 0;
    int iterNumber = 0;
    

    // SetUp
    for (i = 0; i < V; i++) {
        dist[i] = INF;
    }
    dist[src] = 0; // dist for src to src is 0

    #ifdef PARENTS
        for (i = 0; i < V; i++) {
            parent[i] = ABSENTE_EDGE;
        }
    #endif


    

    // Relaxation of the strings |V| - 1 times
    for (count = 0; count < V - 1; count++) {

        toUpdate = 1;
        iterNumber++;

        for (i = 0; i < V; i++) {
            for (j = 0; j < V; j++) {
                if (graph[i][j] != ABSENTE_EDGE) {
                    if(dist[i]!=INF){ // we must avoid adding a value to the maximum representable value, otherwise it will be interpreted as a negative value after the addition
                        temp = dist[i] + graph[i][j];
                        if (dist[j] > temp  ) {
                            dist[j] = temp;

                            #ifdef PARENTS
                                parent[j] = i;
                            #endif

                            toUpdate = 1;

                        }
                    }
                }
            }
        }

    
        // No updates during this iteration, we're exiting early
        if (toUpdate!=1){
            break;
        }
    
    }

    // Check for negative cycles
    if(iterNumber == V-1 ){
        for (i = 0; i < V; i++) {
            for (j = 0; j < V; j++) {
                if (graph[i][j] != ABSENTE_EDGE && dist[i]!=INF && dist[j] > dist[i] + graph[i][j]) {
                    return 1;
                }
            }
        }
    }



    return 0;

    
}

