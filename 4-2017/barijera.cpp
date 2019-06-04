#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <signal.h>
using namespace std;

pthread_mutex_t m;
pthread_cond_t red;
int N, br=0;

void barijera(int i) {
    	int broj;

        pthread_mutex_lock(&m);
        br++;
        if(br < N) {
                cout << "Dretva " << i << ". unesite broj\n";
                cin >> broj;
                pthread_cond_wait(&red, &m);
        }
        else {
                cout << "Dretva " << i << ". unesite broj\n";
                cin >> broj;
                br = 0;
                pthread_cond_broadcast(&red);
        }
        pthread_mutex_unlock(&m);

		pthread_mutex_lock(&m);
        cout << "Dretva " << i << ". uneseni je broj je " << broj << endl;
		pthread_mutex_unlock(&m);
}

void *program(void *x) {
        static int i = 0;
        barijera(i++);
}

void brisi(int sig) {
        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&red);
        exit(0);
}

int main(int argc, char *argv[]) {
        sigset(SIGINT, brisi);

        if(argc!=2) {
                cout << "Krivi unos\n";
                return 0;
        }

        N = atoi(argv[1]);
        if(!N&&cout << "Krivi unos\n") return 0;
        cout << "Broj dretvi=" << N << endl;

        pthread_mutex_init(&m, NULL);
        pthread_cond_init(&red, NULL);
        pthread_t var[N];

        for(int i=0; i<N; i++) pthread_create(&var[i], NULL, program, NULL);
        for(int i=0; i<N; i++) pthread_join(var[i], NULL);

        brisi(0);
        return 0;
}
