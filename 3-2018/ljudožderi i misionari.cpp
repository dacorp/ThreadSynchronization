#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <signal.h>
#include <stdio.h>
using namespace std;

/*
SEMAFORI:

s[0]-kriticni odsjecak
s[1]-cun
s[2]-ukrcavanje
s[3]-iskrcavanje
s[4]-lijeva misionari
s[5]-desna misionari
s[6]-lijeva ljudožde
s[7]-desna ljudižderi
*/

int brojac_lj=0, brojac_m=0, id;
bool polijecemo = false;
sem_t* s;
pthread_t *ljudozder, *misionar, *cun;

int broj[2][2]; //0-lijeva obala 1-desna obala, 0-ljudozder, 1-misionar
int br_m = 0, br_lj = 0;
int obala;
void *m(void *x){
        int o = *((int*) x);

        bool usao = false;
        bool izPrve = true;
        do {
                sem_wait(&s[0]);
                if(obala==o && (br_m+br_lj)<7 && (br_m+1)>=br_lj && !polijecemo) {
                        usao = true;
                        if(!izPrve) broj[o][1]--;
                        br_m++;

                        if((br_m+br_lj)==3) {
                                sem_post(&s[2]);
                                polijecemo = true;
                        }
                }
                else {
                        sem_post(&s[0]);
                        broj[o][1]++;
                        izPrve = false;
                        if(o) sem_wait(&s[5]);
                        else sem_wait(&s[4]);

                }
        }
        while(!usao);
        sem_post(&s[0]);
        sem_wait(&s[1]);
        sem_wait(&s[0]);
        cout << " misionar";
        br_m--;
        if((br_m+br_lj)==0) {
                sem_post(&s[3]);
                polijecemo = false;
        }
        sem_post(&s[0]);
        pthread_exit(NULL);
}
void *lj(void *x){
        int o = *((int*) x);

        bool usao = false;
        bool izPrve = true;

        do {
                sem_wait(&s[0]);
                if(obala==o && (br_m+br_lj)<7 && (br_m>=(br_lj+1) || !(br_m+br_lj) ) && !polijecemo) {
                        usao = true;
                        if(!izPrve) broj[o][0]--;
                        br_lj++;

                        if((br_m+br_lj)==3) {
                                sem_post(&s[2]);
                                polijecemo = true;
                        }
                }
                else {
                        sem_post(&s[0]);
                        broj[o][0]++;
                        izPrve = false;
                        if(o) sem_wait(&s[7]);
                        else sem_wait(&s[6]);
                }
        }
        while(!usao);
        sem_post(&s[0]);
        sem_wait(&s[1]);
        sem_wait(&s[0]);
        cout << " ljudozder";
        br_lj--;
        if((br_m+br_lj)==0) {
                sem_post(&s[3]);
                polijecemo = false;
        }
        sem_post(&s[0]);
        pthread_exit(NULL);
}
void *c(void *x){
        obala = 1;

        while(true) {
                sem_wait(&s[2]);
                sleep(1);
                obala = !obala;
                sleep(1);
                cout << "Prevezeni: ";
                for(int i = 0; i<(br_lj+br_m); i++) {
                        sem_post(&s[1]);
                }
                sem_wait(&s[3]);
                for(int i = 0; i<broj[obala][1]; i++) {
                        if(obala)sem_post(&s[5]);
                        else sem_post(&s[4]);
                }
                for(int i = 0; i<broj[obala][0]; i++) {
                        if(obala)sem_post(&s[7]);
                        else sem_post(&s[6]);
                }
                cout << endl;
        }
}

void kraj(int sig){
        for(int i = 0; i<=8; i++) {
                sem_destroy(&s[i]);
        }
        shmdt((long double*)s);
        shmctl(id,IPC_RMID,NULL);
        exit(0);
        for(int i = 0; i<brojac_lj; i++) {
                pthread_join(ljudozder[i], NULL);
        }
        for(int i = 0; i<brojac_m; i++) {
                pthread_join(misionar[i], NULL);
        }
        pthread_join(cun[0], NULL);
        cout << endl;
}
void shuma(){
        int randomObala;
        srand(time(NULL) ^ (getpid()<<16));
        rand();

        ljudozder = (pthread_t*)malloc(sizeof(pthread_t)*10000);
        misionar = (pthread_t*)malloc(sizeof(pthread_t)*10000);

        while (true) {
                randomObala = rand()%2;
                pthread_create(&ljudozder[brojac_lj], NULL, lj, (void *) &randomObala);
                sleep(1);
                brojac_lj++;
                randomObala = rand()%2;
                pthread_create(&misionar[brojac_m], NULL, m, (void *) &randomObala);
                randomObala = rand()%2;
                brojac_m++;
                pthread_create(&ljudozder[brojac_lj], NULL, lj, (void *) &randomObala);
                sleep(1);
                brojac_lj++;
        }
}

int main(){
        sigset(SIGINT, kraj);

        id = shmget(IPC_PRIVATE,sizeof(int)*100,0600);
        if(id==-1) exit(0);
        s = (sem_t*)shmat(id, NULL, 0);

        for(int i = 0; i<2; i++)
                for(int j = 0; j<2; j++)
                        broj[i][j] = 0;

        sem_init(&s[0], 1, 1);

        cun = (pthread_t*)malloc(sizeof(pthread_t)*2);
        pthread_create(&cun[0], NULL, c, NULL);

        shuma();
        return 0;
}

