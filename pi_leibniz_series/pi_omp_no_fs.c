#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "omp.h"

#define ITERATIONS 2e09
#define REPETITIONS 4
#define PAD 8

struct result {
    double pi;
    long int tv_sec;
    long int tv_usec;
};

int calculate_pi(double **pi_total, int id) {
    int start, end;
    start = (ITERATIONS / omp_get_num_threads()) * id;
    end = (ITERATIONS / omp_get_num_threads()) * (id + 1);
    for (int n = start; n < end; n += 2) {
        pi_total[id][0] = pi_total[id][0] + (double) (4.0 / (2 * n + 1));
        pi_total[id][0] = pi_total[id][0] - (double) (4.0 / (2 * (n + 1) + 1));
        //*(pi_total + id) = *(pi_total + id) + (double) (4.0 / (2 * n + 1));
        //*(pi_total + id) = *(pi_total + id) - (double) (4.0 / (2 * (n + 1) + 1));
    }
    return 0;
}

struct result parallel(int n_threads) {
    double **pi = (double **) malloc(n_threads * sizeof(double*));
    for (int i = 0; i < n_threads; i++) pi[i] = (double *) malloc(PAD * sizeof(double));
    struct timeval t_before, t_after, t_result;
    gettimeofday(&t_before, NULL);
    #pragma omp parallel num_threads(n_threads)
    {
        int id = omp_get_thread_num();
        calculate_pi(pi, id);
    }
    for (int i = 1; i < n_threads; i++) {
        pi[0][0] += pi[i][0];
    }
    gettimeofday(&t_after, NULL);
    timersub(&t_after, &t_before, &t_result);
    struct result r;
    r.pi = pi[0][0];
    r.tv_sec = (long int) t_result.tv_sec;
    r.tv_usec = (long int) t_result.tv_usec;
    return r;
}

struct result sequential() {
    struct timeval t_before, t_after, t_result;
    gettimeofday(&t_before, NULL);
    double res = 0.0;
    for (int n = 0; n < ITERATIONS; n += 2) {
        res = res + (double) (4.0 / (2 * n + 1));
        res = res - (double) (4.0 / (2 * (n + 1) + 1));
    }
    gettimeofday(&t_after, NULL);
    timersub(&t_after, &t_before, &t_result);
    struct result r;
    r.pi = res;
    r.tv_sec = (long int) t_result.tv_sec;
    r.tv_usec = (long int) t_result.tv_usec;
    return r;
}

int main() {
    FILE *seq_file = fopen("results_seq_omp_no_fs.csv", "w");
    if (seq_file == NULL) {
        exit(1);
    }
    fprintf(seq_file, "%s", "r_time;pi\n");
    printf("SEQUENTIAL\n");
    printf("Time\tResult\n");
    for (int it = 0; it < REPETITIONS; it++) {
        struct result r = sequential();
        fprintf(seq_file, "%ld,%06ld;%2.12f\n", r.tv_sec, r.tv_usec, r.pi);
        printf("%ld,%06ld\t%2.12f\n", r.tv_sec, r.tv_usec, r.pi);
    }
    printf("\nPARALLEL\n");
    printf("Threads\tTime\tResult\n");
    for (int it = 0; it < REPETITIONS; it++) {
        char *filename = (char*) malloc(13 * sizeof(char));
        sprintf(filename, "results_omp_no_fs_%d.csv", it);
        FILE *out_file = fopen(filename, "w");
        if (out_file == NULL) {
            exit(1);
        }
        fprintf(out_file, "%s", "n_threads;r_time;pi\n");
        int options[] = {1, 2, 4, 8, 16};
        for (int i = 0; i < 5; i++) {
            struct result r = parallel(options[i]);
            fprintf(out_file, "%d;%ld,%06ld;%2.12f\n", options[i], r.tv_sec, r.tv_usec, r.pi);
            printf("%d\t%ld,%06ld\t%2.12f\n", options[i], r.tv_sec, r.tv_usec, r.pi);
        }
        fclose(out_file);
    }
    return 0;
}