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

#include "toInclude/printCSV.h"


/// @brief This function creates a CSV file at a path specified by the 'type' parameter with the execution times of the programs
/// @param type Type of execution (sequential, MPI, CUDA)
/// @param V 
/// @param opt Optimization used
/// @param numThreads threads OpenMP
/// @param size (MPI: Number of processes), (CUDA: Number of threads)
/// @param total_time 
/// @param algorithm Bellman-Ford execution time
/// @param input_time 
/// @param output_time 
/// @return If errors have occurred
int printCSV(const char type[],int V, int opt, int numThreads, int size, double total_time, double algorithm, double input_time, double output_time){
    FILE *fp;
    FILE *fp2 = NULL; 
    char path[200];
    char path2[200];
    int flag = 0;

    if(strcmp(type, "OMP+MPI") == 0){
        sprintf(path, "Informations/OMP_MPI/opt%d/%dvertices.csv", opt, V); 
    }else if(strcmp(type, "OMP+CUDA") == 0){
        sprintf(path, "Informations/OMP_CUDA/opt%d/%dvertices.csv", opt, V);   
    }else if(strcmp(type, "Sequential") == 0){
        sprintf(path, "Informations/OMP_MPI/opt%d/%dvertices.csv", opt, V);
        sprintf(path2, "Informations/OMP_CUDA/opt%d/%dvertices.csv", opt, V);
        flag = 1;
    }else{
         fprintf(stderr, "Unexpected compilation type: %s\n", type);
         return -2;
    }

    char *filename = path;
    fp = fopen(filename, "a+");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        return -1;
    }

    if(flag ==1){
        char *filename2 = path2;
        fp2 = fopen(filename2, "a+");
            if (fp2 == NULL) {
            fprintf(stderr, "Unable to open file: %s\n", filename2);
            return -1;
        }
    }   
    fprintf(fp, "%s;%d;%d;%06f;%06f;%06f;%06f;\n", type, numThreads, size, total_time, algorithm, input_time, output_time);
    fclose(fp);

    if(fp2!=NULL){
        fprintf(fp2, "%s;%d;%d;%06f;%06f;%06f;%06f;\n", type, numThreads, size, total_time, algorithm, input_time, output_time);
        fclose(fp2);
    }
    
    return 0;
}

/// @brief Useful for passing times to the printCSV function
/// @param time1 
/// @param time2 
/// @return  time elapsed from time1 to time2
double timeval_to_microseconds(struct timeval *time1, struct timeval *time2) {
    unsigned long long temp1 = (unsigned long long)time1->tv_sec * 1000000 + time1->tv_usec;
    unsigned long long temp2 = (unsigned long long)time2->tv_sec * 1000000 + time2->tv_usec;

    return (double)(temp2 - temp1)/(double)1000000;
}

