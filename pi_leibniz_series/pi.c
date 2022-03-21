#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define ITERATIONS 1e09
#define MAX_THREADS 16
#define REPETITIONS 4

int n_threads = 1;
double results[MAX_THREADS];

struct result {
    double pi;
    long int tv_sec;
    long int tv_usec;
};

void *calculate_pi(void *arg) {
    int start, end, thread_id = *(int*) arg;
    start = (ITERATIONS / n_threads) * thread_id;
    end = start + ((ITERATIONS / n_threads) - 1);
    results[thread_id] = 0.0;
    for (int n = start; n <= end; n += 2) {
        results[thread_id] = results[thread_id] + (double) (4.0 / (2 * n + 1));
        results[thread_id] = results[thread_id] - (double) (4.0 / (2 * (n + 1) + 1));
    }
}

struct result parallel() {
    int threads_ids[n_threads];
    pthread_t threads[n_threads];
    struct timeval t_before, t_after, t_result;
    gettimeofday(&t_before, NULL);
    for (int i = 0; i < n_threads; i++) {
        threads_ids[i] = i;
        pthread_create(&threads[i], NULL, calculate_pi, &threads_ids[i]);
    }
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    double pi = 0.0;
    for (int i = 0; i < n_threads; i++) {
        pi += results[i];
    }
    gettimeofday(&t_after, NULL);
    timersub(&t_after, &t_before, &t_result);
    struct result r;
    r.pi = pi;
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
    FILE *seq_file = fopen("results_seq.csv", "w");
    if (seq_file == NULL) {
        exit(1);
    }
    fprintf(seq_file, "%s", "r_time;pi\n");
    for (int it = 0; it < REPETITIONS; it++) {
        struct result r = sequential();
        fprintf(seq_file, "%ld,%06ld;%2.12f\n", r.tv_sec, r.tv_usec, r.pi);
    }
    for (int it = 0; it < REPETITIONS; it++) {
        char *filename = (char*) malloc(13 * sizeof(char));
        sprintf(filename, "results%d.csv", it);
        FILE *out_file = fopen(filename, "w");
        if (out_file == NULL) {
            exit(1);
        }
        fprintf(out_file, "%s", "n_threads;r_time;pi\n");
        int options[] = {1, 2, 4, 8, 16};
        for (int i = 0; i < 5; i++) {
            n_threads = options[i];
            struct result r = parallel();
            fprintf(out_file, "%d;%ld,%06ld;%2.12f\n", n_threads, r.tv_sec, r.tv_usec, r.pi);
        }
        fclose(out_file);
    }
    return 0;
}