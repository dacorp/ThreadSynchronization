#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <signal.h>
using namespace std;
sem_t* s;
int id;
void brisi(int sig){
        shmdt((long double*)s);
        shmctl(id,IPC_RMID,NULL);
        exit(0);
}
void radi(int i){
        int brojac;

        srand(time(NULL) ^ (getpid()<<16));
        rand();
        int trajanje = rand()/(RAND_MAX/10);

        for(brojac=1;brojac<=trajanje;brojac++){
                cout << "Izvodim zadatak " << i << ": " << brojac << "/" << trajanje << endl;
                sleep(1);
        }
}
void proces(int i){
        switch(i){
                case 1:
                        radi(1);
                        sem_post(&s[1]);
                        break;
                case 2:
                        radi(2);
                        sem_post(&s[2]);
                        sem_post(&s[3]);
                        break;
                case 3:
                        sem_wait(&s[2]);
                        radi(3);
                        sem_post(&s[5]);
                        break;
                case 4:
                        sem_wait(&s[3]);
                        radi(4);
                        sem_post(&s[6]);
                        break;
                case 5:
                        sem_wait(&s[1]);
                        radi(5);
                        sem_post(&s[4]);
                        break;
                case 6:
                        sem_wait(&s[5]);
                        radi(6);
                        sem_post(&s[7]);
                        break;
                case 7:
                        sem_wait(&s[4]);
                        sem_wait(&s[7]);
                        sem_wait(&s[6]);
                        radi(7);
                        break;
        }
}
int main(){
        sigset(SIGINT, brisi);
        id = shmget(IPC_PRIVATE,sizeof(int)*100,0600);
        if(id==-1) exit(0);
        s = (sem_t*)shmat(id, NULL, 0);
        for(int i = 1; i<=7; i++) {
                if(fork()==0) {
                        proces(i);
                        exit(0);
                }
        }

        for(int i = 1; i<=7; i++) {
                wait(NULL);
                sem_destroy(&s[i]);
        }
        brisi(0);
        exit(0);
        return 0;
}
