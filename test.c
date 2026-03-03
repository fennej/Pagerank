#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<omp.h>


static long num_steps = 100000000;
double step; 
int vect[8]={1,2,3,4,5,6,7,8};


void fonction(int *a){
    *a=0;
    int somme=0;
    #pragma omp parallel for reduction(+:somme)
    for(int i=0;i<8;i++){
        somme+=vect[i];
    }
    *a=somme;
}


int main(){

    // #pragma omp parallel
    // {
    //     int thread_id = omp_get_thread_num();
    //     printf("Hello from thread %d\n", thread_id);
    // }




    // int i;
    // double x, pi, sum = 0.0;   
    // step = 1.0/(double) num_steps;
    // #pragma omp parallel for reduction(+:sum)
    // for(i=0; i<num_steps; i++){
    //     x = (i + 0.5)*step;
    //     sum = sum + 4.0/(1.0+x*x);
    // }

    // pi = step * sum;
    // printf("Pi = %.12lf\n", pi);








        
//     int i;
//     double x, pi ;   
//     step = 1.0/(double) num_steps;
//     int nbrthreads=0;

// #pragma omp parallel
// {
//     int id = omp_get_thread_num();
//     int nthrds = omp_get_num_threads();
//     double sum = 0.0;
//     double x;   


//     for(int i = id; i < num_steps; i += nthrds){ // ✅ chaque thread prend ses itérations
//         x = (i + 0.5)*step;
//         sum += 4.0/(1.0+x*x);
//     }

//     #pragma omp critical
//     pi += sum;

//     if(id==0){
//         nbrthreads=nthrds;
//     }
// }
// pi *= step;
// printf("Pi = %.12lf\n", pi);
//     printf("Number of threads used: %d\n", nbrthreads);








    // int i;
    // double x, pi, sum = 0.0;   
    // step = 1.0/(double) num_steps;
    // #pragma omp parallel for reduction(+:sum)
    // for(i=0; i<num_steps; i++){
    //     x = (i + 0.5)*step;
    //     sum = sum + 4.0/(1.0+x*x);
    // }

    // pi = step * sum;
    // printf("Pi = %.12lf\n", pi);




    int a = 0;
    #pragma omp parallel 
    {
        int id = omp_get_thread_num();
        int nthrds = omp_get_num_threads();
        fonction(&a);
    }


    printf("a = %d\n", a);


    // htere is an implied barrier at the end of the parallel for, so all threads will have completed their work before we calculate pi and print it.
    // #pragma omp parallel shared(...) pribate (...)
    //#pragma omp barrier 
    //#pragma omp for nowait 
    // # pragma omp master for the master to do things
    // pragma opm single for one thread to do things but there is a barrier in the end on the single construct we cab nowait thogh 


    return 0;
}