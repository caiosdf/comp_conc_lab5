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
int nthreads, dim, *vector, *testVec, narrived;

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

//tarefa executada por cada thread, com o objetivo de retornar cada casa do vetor de entrada como a soma das posições anteriores.
void *sumPrevPositions(void *arg){
    long int id = (long int) arg;
    int watcher; //variável utilizada para observar a posição a ser somada naquela iteração
    
    for(int i = 0; i < dim - 1; i++){
        if((id - pow(2,i)) >= 0) watcher = vector[id - (int)(pow(2,i))];
        else watcher = 0;
        Barrier();
        vector[id] += watcher;
        Barrier();
    }
    
}

//função utilizada para imprimir o vetor
void printVector(){
    printf("[\t");
    for(int i = 0; i < dim; i++){
        printf("%d\t", vector[i]);
    }
    printf("]\n");
}

//cria o vetor de teste
int createTestVec(){
    testVec = (int *)malloc(sizeof(int) * dim);
    if (testVec == NULL)
    {
        puts("ERRO--malloc");
        return 2;
    }
    for(int i = 0; i < dim; i++) testVec[i] = vector[i];
    return 0;
}

int sum(int i){
    if(i == 0) return testVec[0];
    else return testVec[i] + sum(i-1);
}


//função utilizada para testar o vetor
int test(){
    for(int i = 0; i < dim; i++){
        if(vector[i] != sum(i)){
            printf("O somatório na posição %d não é o esperado\n", i);
            return 1;
        }
    }
    puts("O vetor executou os somatorios conforme o esperado.");
    return 0;
}

int main(int argc, char *argv[])
{
    pthread_t *tid;
    
    //verifica o parâmetro de entrada
    if(argc < 2){
        printf("Digite: %s <potência de 2 para determinar o tamanho do vetor>", argv[0]);
        return 1;
    }
    
    //coleta a entrada do teclado
    dim = pow(2,atoi(argv[1]));
    
    nthreads = dim; //atribui a dimensão do vetor ao número de threads
    
    //aloca a memória necessária para o vetor e as threads
    vector = (int *)malloc(sizeof(int) * dim);
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL || vector == NULL)
    {
        puts("ERRO--malloc");
        return 2;
    }
    
    //inicializa o vetor
    for(int i = 0; i < dim; i++){
        vector[i] = rand() % (dim * dim);
    }
    
    printVector();
    if(createTestVec()) return 2;
    
    for (long int i = 0; i < nthreads; i++)
    {
        
        if (pthread_create(tid + i, NULL, sumPrevPositions, (void *) i))
        {
            puts("ERRO--pthread_create");
            return 3;
        }
    }

    for (int i = 0; i < nthreads; i++)
    {
        pthread_join(tid[i], NULL);
    }
    
    if(test()) return 1;
    printVector();
    

    return 0;
}
