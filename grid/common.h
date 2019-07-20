#ifndef _COMMON_H
#define _COMMON_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int rank, size, threads;
#define VISITED     1
#define NOT_VISITED 0
#define NOT_DEFINED -1
#define NUM_TIMERS          4
#define TIMER_SA            0
#define TIMER_ESTIMATED     1
#define TIMER_BFS           2
#define TIMER_CHECK         3

#define MAX_FILENAME_LENGTH 255
#define NUM_OF_PROGRESS     100
#define SKIP_ACCEPTS        10000
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABORT()         do{MPI_Abort(MPI_COMM_WORLD, 1); exit(1);}while(0)
#define PRINT_R0(...)   do{if(rank==0) printf(__VA_ARGS__);}while(0)
#define END(...)        do{if(rank==0) printf(__VA_ARGS__); MPI_Finalize(); exit(0);}while(0)
#define ERROR(...)      do{if(rank==0) printf(__VA_ARGS__); ABORT();}while(0)
#define EXIT(r)         do{MPI_Finalize(); exit(r);}while(0)

extern void swap(int *a, int *b);
extern long long sa(const int nodes, const int lines, double temp, const long long ncalcs,
		    const double cooling_rate, const int low_diam, const double low_ASPL,
		    const bool hill_climbing_flag, const bool detect_temp_flag, double *max_diff_energy,
		    const double max_temp, const double min_temp, int edge[lines][2], int *diam, double *ASPL,
		    const int cooling_cyclie, long long *num_accepts, const int height, int *length,
		    const int low_length, const double weight);
extern void check_current_edge(const int nodes, const int lines, int edge[lines][2], const double low_ASPL);
extern double estimated_elapse_time(const int nodes, const int lines, int edge[lines][2]);
extern bool has_duplicated_edge(const int e00, const int e01, const int e10, const int e11);
extern bool check_loop(const int lines, const int edge[lines][2]);
extern bool check_duplicate_all_edge(const int lines, const int edge[lines][2]);
extern bool check_duplicate_current_edge(const int lines, const int edge[lines][2], const int line[2], int (*tmp_edge)[2]);
extern void create_adjacency(const int nodes, const int lines, const int degree,
                             const int edge[lines][2], int adjacency[nodes][degree]);
extern int getRandom(const int max);
extern void timer_clear_all();
extern void timer_clear(const int n);
extern void timer_start(const int n);
extern void timer_stop(const int n);
extern double timer_read(const int n);
extern bool evaluation(const int nodes, const int lines, const int degree, 
		       int adjacency[nodes][degree], int *diameter, double *ASPL);
extern void edge_copy(int *restrict buf1, const int *restrict buf2, const int n);
#endif
