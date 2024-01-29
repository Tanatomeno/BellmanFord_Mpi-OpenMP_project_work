"""
This file is part of ProjectHPC.
This program is free software: you can redistribute it and/or modify it under the terms of
the GNU General Public License as published by the Free Software Foundation, either version
3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with ContestOMP.
If not, see <https://www.gnu.org/licenses/gpl-3.0.html>.
"""
import os
import shutil

def analizeFile(sourceFile,resultFile):
    f=open(sourceFile,"r")
    lines=f.readlines()
    data={}
    num={}
    for line in lines:
        type,omp,mpi,totalTime,algorithmTime,inputTime,outputTime=line.strip()[:-1].split(";")
        if type not in data:
            data[type]={}
            num[type]={}
        if omp not in data[type]:
            data[type][omp]={}
            num[type][omp]={}
        if mpi not in data[type][omp]:
            data[type][omp][mpi]=[float(totalTime),float(algorithmTime),float(inputTime),float(outputTime)]
            num[type][omp][mpi]=1
        else:
            data[type][omp][mpi][0]+=float(totalTime)
            data[type][omp][mpi][1]+=float(algorithmTime)
            data[type][omp][mpi][2]+=float(inputTime)
            data[type][omp][mpi][3]+=float(outputTime)
            num[type][omp][mpi]+=1
    f.close()
    f=open(resultFile,"w")
    f.write("Type;OMP_threads;MPI_process;total_time;algorithm;input_time;output_time;speedup;efficency\n")
    for typeKey in data.keys():
        ompList=list(data[typeKey].keys())
        ompList.sort()
        for ompKey in ompList:
            mpiList=list(data[typeKey][ompKey].keys())
            mpiList.sort()
            for mpiKey in mpiList:
                it=num[typeKey][ompKey][mpiKey]
                data[typeKey][ompKey][mpiKey][0]/=it
                data[typeKey][ompKey][mpiKey][1]/=it
                data[typeKey][ompKey][mpiKey][2]/=it
                data[typeKey][ompKey][mpiKey][3]/=it
                t_parallel0 = data["Sequential"]["0"]["0"][0]
                speedup = t_parallel0 / data[typeKey][ompKey][mpiKey][0]
                if typeKey != "Sequential":
                    efficiency = t_parallel0/(int(ompKey)*int(mpiKey)*data[typeKey][ompKey][mpiKey][0])*100
                else:
                    t_parallel0 = 1
                    efficiency = 100
                formatted_line = (
                    f"{typeKey};{ompKey};{mpiKey};"
                    f"{data[typeKey][ompKey][mpiKey][0]:.6f};"
                    f"{data[typeKey][ompKey][mpiKey][1]:.6f};"
                    f"{data[typeKey][ompKey][mpiKey][2]:.6f};"
                    f"{data[typeKey][ompKey][mpiKey][3]:.6f};"
                    f"{speedup:.6f};{efficiency:.6f}\n"
                )
                f.write(formatted_line)
    f.close()

if os.path.exists("Results"):
    shutil.rmtree('Results')

os.mkdir("Results")

for path, currentDirectory, files in os.walk("Informations/OMP_MPI"):
    for file in files:
        if file.lower() == "desktop.ini":
            continue
        resultPath=path.replace("Informations","Results")
        if not(os.path.exists(resultPath)):
            os.makedirs(resultPath)
        analizeFile(os.path.join(path, file),os.path.join(resultPath,file))