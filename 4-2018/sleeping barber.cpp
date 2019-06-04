#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

pthread_mutex_t m;
pthread_cond_t b, red;
bool otvoreno, krajRadnogVremena = false, spava=false;
int br_mjesta = 0;

void *brijac(void *x) {
        pthread_mutex_lock(&m);
        otvoreno = true;
        cout << "Brijac: Otvaram salon\n";
        pthread_mutex_unlock(&m);
        while(true) {
                pthread_mutex_lock(&m);
                if(krajRadnogVremena) {
                        otvoreno = false;
                }
                if(br_mjesta>0) {
                        cout << "Brijac: Idem raditi\n";
                        pthread_cond_signal(&red);
                        pthread_mutex_unlock(&m);
                        sleep(3);
                        pthread_cond_wait(&b, &m);
                        cout << "Brijac: Klijent gotov\n";
                }
                else if(otvoreno) {
                        br_mjesta = 0;
                        cout << "Brijac: Spim dok klijenti ne dodju\n";
                        pthread_mutex_unlock(&m);
                        spava = true;
                        pthread_cond_wait(&b, &m);
                        spava = false;
                }
                else {
                        cout << "Brijac: Zatvaram salon\n";
                        pthread_mutex_unlock(&m);
                        break;
                }
                pthread_mutex_unlock(&m);
        }
}
void *klijent(void *x) {
        int rbKlijent = *((int*) x);
        pthread_mutex_lock(&m);
        cout << "\tKlijent(" << rbKlijent << "): Zelim brijanje\n";
        if(otvoreno && br_mjesta < 3) {
                br_mjesta++;
                cout << "\tKlijent(" << rbKlijent << "): Ulazim u cekaonu (" << br_mjesta  << ")\n";
                if(spava) pthread_cond_signal(&b);
                pthread_mutex_unlock(&m);
                pthread_cond_wait(&red, &m);
                cout << "\tKlijent(" << rbKlijent << "): brijac mi radi brijanje\n";
                pthread_mutex_lock(&m);
                br_mjesta--;
                pthread_cond_signal(&b);
        }
        else {
                cout << "\tKlijent(" << rbKlijent << "): Nista od brijanja danas\n";
        }
        pthread_mutex_unlock(&m);
}

void signal(int sig){
        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&red);
        pthread_cond_destroy(&b);
        exit(0);
}
int main(){
        sigset(SIGINT, signal);

        pthread_mutex_init(&m, NULL);
        pthread_cond_init(&red, NULL);
        pthread_cond_init(&b, NULL);
        pthread_t id_brijac, id_klijent[10];

        pthread_create(&id_brijac, NULL, brijac, NULL);
        for(int i=0; i<9; i++) {
                pthread_create(&id_klijent[i], NULL, klijent, (void*)&i);
                sleep(1);
        }

        krajRadnogVremena = true;
        pthread_join(id_brijac, NULL);
        for(int i = 0; i<7; i++) {
                pthread_join(id_klijent[i], NULL);
        }
        signal(0);
        return 0;
}

