#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <signal.h>
using namespace std;
struct podaci{
        int ULAZ,IZLAZ, KRAJ,M[5];
};
podaci *x;
int id, id2, id3, *SemId, broj_slucajnih, broj_procesa;

void SemGet(int n) {
        *SemId = semget(IPC_PRIVATE, n, 0600);
        if (*SemId == -1) {
                cout << "Nema semafora!\n";
                exit(1);
        }
        id3 = (int)(*SemId);
}

int SemSetVal(int SemNum, int SemVal) {
        return semctl(*SemId, SemNum, SETVAL, &SemVal);
}

int SemOp(int SemNum, int SemOp){
        struct sembuf SemBuf;
        SemBuf.sem_num = SemNum;
        SemBuf.sem_op  = SemOp;
        SemBuf.sem_flg = 0;
        return semop(*SemId, & SemBuf, 1);
}

void SemRemove(void) {
        (void) semctl(id3, 0, IPC_RMID, 0);
}

void brisi(int sig){
        shmdt((podaci*)x);
        shmdt((int*)SemId);
        shmctl(id,IPC_RMID,NULL);
        shmctl(id2,IPC_RMID,NULL);
        SemRemove();
        exit(0);
}

void proizvodjac(int brojac){
        int broj;
        srand(time(NULL) ^ (getpid()<<16));
        rand();
        for(int i = 0; i<broj_slucajnih; i++){
                usleep(100);
                if(i!=0) {
                        SemOp(0,-1);
                        SemOp(1,-1);
                }

                broj = rand()/(RAND_MAX/1000);
                cout << "Proizvodjac " << brojac << " salje '" << broj << "'" << endl;

                x->M[x->ULAZ] = broj;
                x->ULAZ = (x->ULAZ+1)%5;
                SemOp(1,1);
                SemOp(2,1);
        }
        cout << "Proizvodjac " << brojac << " zavrsio sa slanjem\n";
}

void potrosac(){
        int suma=0;
        for(int i = 0; i<(broj_procesa*broj_slucajnih); i++){
                SemOp(2,-1);
                cout << "Potrosac prima " << x->M[x->IZLAZ] << endl;
                suma += x->M[x->IZLAZ];
                x->IZLAZ = (x->IZLAZ + 1) % 5;
                SemOp(0,1);
        }
        cout << "Potrosac - suma primljenih brojeva=" << suma << endl;
}

int main(int argc, char*argv[]){
        if(argc<3) {
                cout << "Krivi unos!" << endl;
                return 0;
        }

        broj_procesa = atoi(argv[1]);
        broj_slucajnih = atoi(argv[2]);

        if(!broj_procesa || !broj_slucajnih) {
                cout << "Unesi broj parametara preko komandne linije!\n";
                return 0;
        }
        sigset(SIGINT, brisi);

        id = shmget(IPC_PRIVATE,sizeof(podaci),0600);
        if(id==-1) exit(0);
        x = (podaci*) shmat(id,NULL,0);

        id2 = shmget(IPC_PRIVATE,sizeof(int),0600);
        if(id2==-1) exit(0);
        SemId = (int*)shmat(id2,NULL,0);
        SemGet(3);
        SemSetVal(0,1); //PUN
        SemSetVal(1,1); //PIŠI
        SemSetVal(2,-1); //PRAZAN

        for(int i = 0; i<=broj_procesa; i++){
                if(fork()==0){
                        if(!i) potrosac();
                        else proizvodjac(i);
                        exit(0);
                }
        }
        for(int i = 0; i<=broj_procesa; i++) wait(NULL);
        brisi(0);
        exit(0);
        return 0;
}
