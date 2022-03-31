import os
import sys
import numpy as np
import multiprocessing
import matplotlib.pyplot as plt

def generate_plots(exec_type):
    threads = []
    seq_times = []
    par_times = {}
    for element in os.listdir(os.getcwd()):
        if '.csv' not in element:
            continue
        if 'seq' in element and exec_type in element:
            with open(element, 'r', encoding='utf-8') as csv_file:
                for line in csv_file.readlines()[1:]:
                    seq_times.append(float(line.split(';')[0].replace(',', '.')))
            print(f'{element} processed')
        elif exec_type in element:
            with open(element, 'r', encoding='utf-8') as csv_file:
                for line in csv_file.readlines()[1:]:
                    n_threads = int(line.split(';')[0])
                    if n_threads not in threads:
                        threads.append(n_threads)
                    r_time = float(line.split(';')[1].replace(',', '.'))
                    if n_threads not in par_times.keys():
                        par_times[n_threads] = []
                    par_times[n_threads].append(r_time)
            print(f'{element} processed')
    seq_time = np.average(seq_times)
    par_avg_times = []
    for thread in threads:
        par_avg_times.append(np.average(par_times[thread]))
    speed_up = [seq_time / par_time for par_time in par_avg_times]
    fig1, ax1 = plt.subplots()
    ax1.plot(threads, par_avg_times)
    ax1.set(xlabel='Number of threads', ylabel='Response time (s)', title='N° of Threads vs Response Time')
    ax1.grid()
    fig1.savefig(f"time_{exec_type}.png")
    fig2, ax2 = plt.subplots()
    ax2.plot(threads, speed_up)
    ax2.set(xlabel='Number of threads', ylabel='Speed up (s)', title='N° of Threads vs Speed Up')
    ax2.grid()
    fig2.savefig(f"speedup_{exec_type}.png")

if __name__ == '__main__':
    types = ['pthread', 'omp_fs', 'omp_no_fs']
    exec_type = sys.argv[1]
    if exec_type not in types:
        print('Option is not available')
        sys.exit()
    generate_plots(exec_type)
    # print(multiprocessing.cpu_count())