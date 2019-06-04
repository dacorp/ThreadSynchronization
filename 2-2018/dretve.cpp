#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <stdio.h>
using namespace std;
int *polje;
const int N = 30;

void *racunaj(void *x) {
        int dretva = *((int*) x);
        int suma = 0;
        double avg = 0;

        for(int i = dretva*N; i<(dretva+1)*N; i++) suma += polje[i];
        printf("polje %i suma= %i \n", (dretva+1), suma);

        sleep(1);

        avg = (double)suma/N;
        printf("polje %i aritm_sredina= %f \n", (dretva+1), avg);
}

void *generiraj(void *x) {
        int dretva = *((int*) x);

        for(int i = dretva*N; i<(dretva+1)*N; i++) {
                polje[i] = rand()%5000;
        }
}
int main(int argc, char *argv[]) {
        int i, M;
        pthread_t *prviTip, *drugiTip;

        srand(time(0));
        rand();

        if (argc<2) {
                cout << "Krivi unos\n";
                return 0;
        }
        M = atoi(argv[1]);
        if (M<=0) {
                cout << "Krivi unos\n";
                return 0;
        }

        polje = (int*)malloc(sizeof(int)*M*N);

        prviTip = (pthread_t*)malloc(sizeof(pthread_t)*M);
        for (i=0; i<M; i++) pthread_create(&prviTip[i], NULL, generiraj, (void *) &i);

        drugiTip = (pthread_t*)malloc(sizeof(pthread_t)*M);
        for (i=0; i<M; i++) pthread_create(&drugiTip[i], NULL, racunaj, (void *) &i);
        for (i=0; i<M; i++) {
                pthread_join(prviTip[i], NULL);
                pthread_join(drugiTip[i], NULL);
        }

        return 0;
}
