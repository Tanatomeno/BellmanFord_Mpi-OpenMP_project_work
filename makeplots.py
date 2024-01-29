"""
Copyright (C) 2024 - All Rights Reserved
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
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D


results_folder = 'Results'
plots_folder = 'Plots'


os.makedirs(plots_folder, exist_ok=True)


for root, dirs, files in os.walk(results_folder):
    for dir_name in dirs:
        if 'OMP_MPI' in dir_name:
            omp_mpi_folder = os.path.join(root, dir_name)


            for opt_folder in ['opt0', 'opt1', 'opt2', 'opt3']:
                opt_folder_path = os.path.join(omp_mpi_folder, opt_folder)


                csv_files = [f for f in os.listdir(opt_folder_path) if f.endswith('.csv')]

                for csv_file in csv_files:

                    file_path = os.path.join(opt_folder_path, csv_file)
                    df = pd.read_csv(file_path, delimiter=';')


                    omp_mpi_data = df[df['Type'] == 'OMP+MPI']
                    omp_thread_counts = sorted(set(omp_mpi_data['OMP_threads']))
                    mpi_process_counts = sorted(set(omp_mpi_data['MPI_process']))


                    plot_title = os.path.splitext(csv_file)[0]


                    plt.figure(figsize=(12, 8), dpi=300)


                    omp_legend_set = set()


                    colors = ['blue','green', 'red', 'purple', 'orange','brown']
                    legend_handles = []
                    for mpi_processes, color in zip(mpi_process_counts, colors):
                        label = f'MPI_processes={mpi_processes}' 

                        if label not in omp_legend_set:
                            omp_legend_set.add(label)
                            line = Line2D([0], [0], marker='o', color=color, label=label)
                            legend_handles.append(line)
                        else:
                            label = "_nolegend_" 

                        for omp_threads in omp_thread_counts:
                            subset = omp_mpi_data[
                                (omp_mpi_data['OMP_threads'] == omp_threads) & (omp_mpi_data['MPI_process'] == mpi_processes)]
                            speedup_values = subset['speedup'].tolist()
                            plt.plot(subset['OMP_threads'], speedup_values, marker='o', linestyle='-', color=color)

                    # plot option
                    plt.title(f'{plot_title}', fontsize=24)  
                    plt.xlabel('Number of thread (openMP_threads)')
                    plt.ylabel('Speedup')
                    plt.xticks(mpi_process_counts)

                    plt.ylim(0, 5)


                    plt.grid(True)


                    plt.subplots_adjust(top=0.9)


                    plt.legend(handles=legend_handles, loc='upper left')
                    for mpi_processes, color in zip(mpi_process_counts, colors):
                        mpi_subset = omp_mpi_data[omp_mpi_data['MPI_process'] == mpi_processes]
                        mpi_speedup_values = mpi_subset.groupby('OMP_threads')['speedup'].mean().tolist()
                        plt.plot(mpi_subset.groupby('OMP_threads')['OMP_threads'].mean(), mpi_speedup_values, marker='', linestyle='-',
                                color=color)


                    output_folder = os.path.join(plots_folder, dir_name, opt_folder)
                    os.makedirs(output_folder, exist_ok=True)
                    output_file_path = os.path.join(output_folder, f"{plot_title}.jpg")
                    plt.savefig(output_file_path, format='jpg')
                    plt.close()
