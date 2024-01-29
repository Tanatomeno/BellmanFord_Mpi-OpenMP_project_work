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


#include "../toInclude/BellmanFord/bellmanFord_parallel.h"

//To be defined if we also want to find the path of parents
//#define PARENTS 


/// @brief  Parallelization of the Bellman-Ford algorithm with OpenMP and MPI. 
///         In this version, in addition to finding the minimum path, it also stores the traversed vertices
/// @param graph The 1D graph to work on
/// @param V Number of vertices in the graph
/// @param src Starting vertex identified by a number
/// @param dist Vector of distances passed from the main
/// @param parent Vector of parents passed from the main
/// @param comm Communicator to parallelize on
/// @param myStart Starting index of this process.
/// @param myEnd Ending index of this process.
/// @return Presence of negative cycles
int bellmanFord_parallel(int* graph, int V, int src, int * dist, int* parent, MPI_Comm comm, int myStart, int myEnd) {

    int temp, count;
    int toUpdate = 0;
    int iterNumber = 0;
    int hasNegativeCycle = 0;
    int i,j;

    //MPI
    int myRank;
    MPI_Comm_rank(comm, &myRank);
    
    #ifdef PARENTS
        int * oldDist
        int x = INF;
    #endif
    //---

        
    //OPEN_MP!
    #pragma omp parallel for default(none) private(i) shared(V, dist) 
    for (i = 0; i < V; i++) {
        dist[i] = INF;
    }

    dist[src] = 0; // dist for src to src is 0

    #ifdef PARENTS
        //OPEN_MP! 
        #pragma omp parallel for default(none) private(i) shared(V, parent) 
        for (i = 0; i < V; i++) {
            parent[i] = ABSENTE_EDGE;
        }
        
        int * oldDist = (int*)malloc(V * sizeof(int));
        if(oldDist == NULL){
            fprintf(stderr,"malloc error\n");
            MPI_Finalize();
        return -1;
        }

    #endif


    
    //Evryone Starting at the same (not essentia)
    MPI_Barrier(comm);


    // Relaxation of the strings |V| - 1 times
    for (count = 0; count < V - 1; count++) { 

        toUpdate = 0; //Variable indicating whether it is necessary to proceed with further iterations
        iterNumber ++;
        
        //OpenMP!
        #ifdef PARENTS
        #pragma omp parallel for default(none) private(i,j,temp) shared(parent,oldDist,dist,graph,V,myStart,myEnd) reduction (+:toUpdate)
        #else
        #pragma omp parallel for default(none) private(i,j,temp) shared(parent,dist,graph,V,myStart,myEnd) reduction (+:toUpdate)
        #endif
        for (i = 0; i < myEnd-myStart; i++) { // <-- MPI! (division of matrix)
            for (j = 0; j < V; j++) {
                if (graph[i*V+j] != ABSENTE_EDGE) {
                    if(dist[myStart+i]!=INF){ 
                        temp = dist[myStart+i] + graph[i*V+j];
                        if (dist[j] > temp) {
                            dist[j] = temp;

                            #ifdef PARENTS
                                oldDist[j] = temp;
                                parent[j] = myStart+i;
                            #endif

                            toUpdate++;

                        }
                    }
                }
            }
        }

        //MPI!
        MPI_Allreduce(MPI_IN_PLACE, &toUpdate, 1, MPI_INT, MPI_MAX,comm);  // we check if any process requests to continue

        if (toUpdate==0){
            break;
        }         
        

        
        //MPI!
        MPI_Allreduce(MPI_IN_PLACE, dist, V, MPI_INT, MPI_MIN, comm); //we obtain the distance vector by assuming the minimum distances found by the processes are correct

        #ifdef PARENTS
            //MPI! (Segment that allows us to obtain the parent vector as well)
            for (j=0; j<V; j++){
                if(dist[j]!=INF && dist[j]==oldDist[j]){ //Each process checks if it was the one to find the minimum path
                    x=myRank; 
                }
                else{
                    x=INF;
                }

                MPI_Allreduce(MPI_IN_PLACE, &x, 1, MPI_INT, MPI_MIN, comm);//The process that found the minimum path is communicated (if it has happened)
                
                if(x!=INF){ //If at least one process has found a path
                    MPI_Bcast(&parent[j],1,MPI_INT,x,comm); //That process will send to all other processes the parent of the node reached with the minimum path
                }

                oldDist[j]=INF;
            }
        #endif
           
    }



    // Simply perform another iteration and check if an even smaller path is found (which is not possible if there are no negative cycles)
    if(iterNumber==V-1){
        hasNegativeCycle = 0;
        //OpenMP!
        #pragma omp parallel for default(none) private(i,j) shared(dist,graph,V,myStart,myEnd) reduction(+:hasNegativeCycle)
        for (i = 0; i < myEnd-myStart; i++) {
            for (j = 0; j < V; j++) {
                if (graph[i*V+j] != ABSENTE_EDGE && dist[myStart+i]!=INF && dist[j] > dist[myStart+i] + graph[i*V+j]) {
                    hasNegativeCycle++;
                }
            }
        }
        //MPI!
        MPI_Allreduce(MPI_IN_PLACE, &hasNegativeCycle, 1, MPI_INT, MPI_MAX, comm);
    }
    if(hasNegativeCycle>0){
        hasNegativeCycle=1;
    }

    #ifdef PARENTS
        free(oldDist);
    #endif

    return hasNegativeCycle;   
}



