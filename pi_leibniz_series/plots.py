import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def generate_plots():
    threads = []
    seq_times = []
    par_times = {}
    for element in os.listdir(os.getcwd()):
        if '.csv' not in element:
            continue
        if 'seq' in element:
            with open(element, 'r', encoding='utf-8') as csv_file:
                for line in csv_file.readlines()[1:]:
                    seq_times.append(float(line.split(';')[0].replace(',', '.')))
        else:
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
    fig1.savefig("time.png")
    fig2, ax2 = plt.subplots()
    ax2.plot(threads, speed_up)
    ax2.set(xlabel='Number of threads', ylabel='Speed up (s)', title='N° of Threads vs Speed Up')
    ax2.grid()
    fig2.savefig("speed_up.png")

generate_plots()