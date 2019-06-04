#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
using namespace std;
int  N, *TRAZIM, *BROJ;

void udji_u_kriticni_odsjecak(int i) {
        int j;
        TRAZIM[i] = 1;
        for (j=0; j<N; j++) if (BROJ[j]>BROJ[i]) BROJ[i]=BROJ[j];
        BROJ[i]++;
        TRAZIM[i] = 0;

        for (j=0; j<N; j++)  {
                while (TRAZIM[j] != 0);
                while (BROJ[j] !=0 && (BROJ[j]<BROJ[i] || (BROJ[j]==BROJ[i] && j<i)));
        }
}

void izadji_iz_kriticnog_odsjecka(int i) {
        BROJ[i] = 0;
}

int novi() {
        static int i=0;
        return i++;
}

void *algoritam(void *x) {
        int k, m, i=novi();
        for (k=1; k<=5; k++) {
                udji_u_kriticni_odsjecak(i);
                for (m=1; m<=5; m++) {
                        cout << "Dretva " << i+1 << ", K.O. br: " << k << " (" <<m << "/5)\n";
                        sleep(1);
                }
                izadji_iz_kriticnog_odsjecka(i);
        }
}

int main(int argc, char *argv[]) {
        int i;
        pthread_t *var;
        if (argc<2) {
                cout << "Krivi unos\n";
                return 0;
        }
        N = atoi(argv[1]);
        if (!N) {
                cout << "Krivi unos\n";
                return 0;
        }
        var = (pthread_t*)malloc(sizeof(pthread_t)*N);
        TRAZIM = (int *)malloc(sizeof(int)*N);
        BROJ = (int *)malloc(sizeof(int)*N);

        for (i=0; i<N; i++) pthread_create(&var[i], NULL, algoritam, NULL);
        for (i=0; i<N; i++) pthread_join(var[i], NULL);
        return 0;
}
