/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

pthread_mutex_t barrier;
pthread_cond_t go;
int nthreads, dim, *vector, narrived;

void Barrier(){
    pthread_mutex_lock(&barrier);
    narrived++;
    if(narrived == nthreads){
        narrived = 0;
        pthread_cond_broadcast(&go);
    }else {
        pthread_cond_wait(&go, &barrier);
    }
    pthread_mutex_unlock(&barrier);
}

void *sumPrevPositions(void *arg){
    int *id = (int *) arg;
    int watcher; //variável utilizada para observar a posição a ser somada naquela iteração
    
    for(int i = 0; i < dim - 1; i++){
        if((*id - pow(2,i)) >= 0) watcher = vector[*id - (int)(pow(2,i))];
        else watcher = 0;
        Barrier();
        vector[*id] += watcher;
    }
    
}

void printVector(){
    printf("[\t");
    for(int i = 0; i < dim; i++){
        printf("%d\t", vector[i]);
    }
    printf("]\n");
}

int main(int argc, char **argv)
{
    pthread_t *tid;
    int id;
    
    //coleta a entrada do teclado
    dim = pow(2,atoi(argv[1]));
    if(argv < 2){
        printf("Digite: %s <potência de 2 para determinar o tamanho do vetor>", argv[0]);
        return 1;
    }
    
    nthreads = dim; //atribui a dimensão do vetor ao número de threads
    
    //aloca a memória necessária para o vetor e as threads
    vector = (int *)malloc(sizeof(int) * dim);
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL)
    {
        puts("ERRO--malloc");
        return 2;
    }
    
    //inicializa o vetor
    for(int i = 0; i < dim; i++){
        vector[i] = rand() % (dim * dim);
    }
    
    printVector();
    
    for (int i = 0; i < nthreads; i++)
    {
        id = i;
        
        if (pthread_create(tid + i, NULL, sumPrevPositions, (void *) id));
        {
            puts("ERRO--pthread_create");
            return 3;
        }
    }

    for (int i = 0; i < nthreads; i++)
    {
        pthread_join(tid[i], NULL);
    }
    
    printVector();
    

    return 0;
}
